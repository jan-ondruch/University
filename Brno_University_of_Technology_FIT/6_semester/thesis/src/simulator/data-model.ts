var fs = require('fs');
const path = require('path');

// 20 tags simulator: _tag_id: tag_id - 1
let data = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]];

/* set data to @data array from the files */
exports.setData = () => {
  fs.readFile(path.resolve(__dirname, './data/rawdata_d12.js'), 'utf8', (err, dat) => {
    if (err) throw err;
    
    dat = JSON.parse(dat);
    data[11] = dat.data;
  });

  fs.readFile(path.resolve(__dirname, './data/rawdata_d13.js'), 'utf8', (err, dat) => {
    if (err) throw err;
    
    dat = JSON.parse(dat);
    data[12] = dat.data;
  });

  fs.readFile(path.resolve(__dirname, './data/rawdata_d14.js'), 'utf8', (err, dat) => {
    if (err) throw err;
    
    dat = JSON.parse(dat);
    data[13] = dat.data;
  });

  fs.readFile(path.resolve(__dirname, './data/rawdata_d15.js'), 'utf8', (err, dat) => {
    if (err) throw err;
    
    dat = JSON.parse(dat);
    data[14] = dat.data;
  });

  fs.readFile(path.resolve(__dirname, './data/rawdata_d16.js'), 'utf8', (err, dat) => {
    if (err) throw err;
    
    dat = JSON.parse(dat);
    data[15] = dat.data;
  });
}

exports.data = data;