// constructor function
exports.playerData = function(player_id) {
  this.player_id = player_id; //
  this.firstname = null; //
  this.lastname = null; //
  this.age = 0; //
  this.tag_id = 0; //
  this.hr_curr = 0; //
  this.hr_percentage = 0; //
  this.distance = 0; //
  this.sprint_distance = 0;
  this.speed_zones = []; //
  this.hr_zones = []; //
};

// speed | hr zone
exports.zone = function(value) {
  this.value = value;
  this.count = 0;
  this.percentage = 0;
};