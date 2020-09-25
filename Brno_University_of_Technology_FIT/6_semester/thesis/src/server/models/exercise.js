exercise = function(exercise_id, name, start, end, session_id) {
  this.exercise_id = exercise_id;
  this.name = name;
  this.start = start;
  this.end = end;
  this.session_id = session_id;
};

module.exports = exercise;