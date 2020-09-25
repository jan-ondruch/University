player = function(player_id, firstname, lastname, birth, weight, tag_id, removed, team_id) {
  this.player_id = player_id;
  this.firstname = firstname;
  this.lastname = lastname;
  this.birth = birth;
  this.weight = weight;
  this.tag_id = tag_id;
  this.removed = removed;
  this.team_id = team_id;
};

module.exports = player;