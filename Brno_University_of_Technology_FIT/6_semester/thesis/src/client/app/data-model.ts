export class Account {
	email: string;
	password: string
}

export class Team {
	team_id: number;
	name: string;
	email: string
}

export class Player {
	player_id: number;
	firstname: string;
	lastname: string;
	birth: string;
	weight: number;
	tag_id: number;
	team_id: number;
}

export class Session {
	session_id: number;
	name: string;
	start: string;
	end: string;
	team_id: number;
}

export class Exercise {
	exercise_id: number;
	name: string;
	start: string;
	end: string;
	session_id: number;
}

export class Settings {
  settings_id: number;
  server: string;
  port: string;
  api_key: string;
}