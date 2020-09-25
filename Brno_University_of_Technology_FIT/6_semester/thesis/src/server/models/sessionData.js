const HR_ZONES = [0, 114, 133, 152, 172, 200];
const SPEED_ZONES = [0, 1, 2, 30];
const HR_INIT = 220;
const HR_MAX = 200; // averaged value
const SPRINT_THRESHOLD = SPEED_ZONES[2];
const SESSION_TIMEOUT = 10800000; // 3 hours
const MAX_SPEED = 7.0;  // guard for skewed data

let data_sampling_freq = 1000;

module.exports = {
  HR_ZONES,
  SPEED_ZONES,
  HR_INIT,
  HR_MAX,
  SPRINT_THRESHOLD,
  SESSION_TIMEOUT,
  MAX_SPEED,
  data_sampling_freq
};