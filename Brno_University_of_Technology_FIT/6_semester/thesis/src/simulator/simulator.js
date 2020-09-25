const WebSocketServer = require("ws").Server;
var moment = require('moment');

const port = 4000;
const wss = new WebSocketServer({ port: port });

// import data & set tag data from the files
const data_model = require('./data-model.ts');
data_model.setData();

let data = data_model.data;


let wscons = [];  // all connected websockets


/* new ws connection */
wss.on("connection", (ws) => {
  // create a new connection instance and store it into an array
  let wscon = new WSConnection(ws);
  wscons.push(wscon);
});


/*
 * Unique identifier generator.
 * Counter for WSConnection (wscons).
 */
let ws_id = 0;
let set_ws_id = () => {
  ws_id ++;
  return ws_id;
}


/* 
 * WebSocket connection.
 * Takes care of one WS connection - receives/sends data, handles the conn.
 */
class WSConnection {
  
  constructor(ws) {
    this.ws = ws;
    this.tag_id;
    this.ws_id = set_ws_id(); // unique identifier
    this.alive = true;        // until the socket is connected

    this.message(); // wait for the greeting message with the tag_id
    this.close();   // watch for socket disconnection
  }

  /* 
   * Send data.
   * If tag doesn't exist, send false and close the connection and remove it.
   */
  send() {
    if (data[this.tag_id-1] !== undefined && data[this.tag_id-1].length !== 0) {
      this.sendInLoop(0); // start sending data
    } else {
      this.ws.send(JSON.stringify({data: false}));
      this.ws.close();
      this.removeFromCons();
    }
  }

  /* 
   * Send data every timestamp-diff seconds
   */
  sendInLoop(i) {
    if (i >= data[this.tag_id-1].length-1) {
      data[this.tag_id-1][i].body.datastreams[0].at = 
        moment().format('YYYY-MM-DD HH:mm:ss.SSSSSS'); // set the last element
      
      this.sendInLoop(0); // start all over again
      return;             // cancel this function
    }

    // calculate timestamps diff
    let t1 = moment(data[this.tag_id-1][i].body.datastreams[0].at);
    let t2 = moment(data[this.tag_id-1][i+1].body.datastreams[0].at);
    let t1_t2 = t2 - t1;

    // rewrite to timestamp to the current time
    data[this.tag_id-1][i].body.datastreams[0].at = 
      moment().format('YYYY-MM-DD HH:mm:ss.SSSSSS');

    // stop sending data
    if (!this.alive) return;

    // send the data
    setTimeout(() => {
      if (this.ws.readyState === this.ws.OPEN) {
        this.ws.send(JSON.stringify(data[this.tag_id-1][i]));
      }

      this.sendInLoop(++i); // call the function again
    }, t1_t2);
  }
 
  /* 
   * On message 
   */
  message() {
    let tag_id;

    // get tag_id from the client initial message
    let tag_promise = new Promise((resolve, reject) => {
      this.ws.onmessage = (message) => {
        
        // parse out tag_id
        tag_id = message.data.substring(message.data.indexOf('/feeds/'));
        tag_id = tag_id.substring(7, tag_id.length - 2);

        resolve();
      }
    });

    // start sending data
    tag_promise.then(() => {
      this.tag_id = tag_id;
      this.send();
    });
  }

  /* 
   * Return ws_id.
   */
  getWsId() {
    return this.ws_id;
  }

  /* 
   * On close.
   */
  close() {
    let close_promise = new Promise((resolve, reject) => {
      this.ws.onclose = () => {
        this.alive = false; // stop sending data
        resolve();
      }
    });

    close_promise.then(() => {
      this.removeFromCons();
    });
  }

  /* 
   * Remove the closed connection from the active connections.
   */
  removeFromCons() {
    wscons = wscons.filter((wsc) => wsc.getWsId() !== this.ws_id);
  }
}