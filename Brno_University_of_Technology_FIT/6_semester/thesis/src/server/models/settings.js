settings = function(settings_id, server, port, api_key, email) {
  this.settings_id = settings_id;
  this.server = server;
  this.port = port;
  this.api_key = api_key;
  this.email = email;
};

module.exports = settings;