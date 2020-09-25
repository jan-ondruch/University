session = function(session_id, name, start, end, team_id) {
  this.session_id = session_id;
  this.name = name;
  this.start = start;
  this.end = end;
  this.team_id = team_id;
};

module.exports = session;