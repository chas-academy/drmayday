Dotenv.config();

let env = Node.Process.process##env;

let slackToken = Js.Dict.unsafeGet(env, "SLACK_TOKEN");
let slackClientId = Js.Dict.unsafeGet(env, "CLIENT_ID");
let slackClientSecret = Js.Dict.unsafeGet(env, "CLIENT_SECRET");

type db = {
  host: string,
  name: string,
  user: string,
  password: string,
};
let db = {
  host: Js.Dict.unsafeGet(env, "DB_HOST"),
  name: Js.Dict.unsafeGet(env, "DB_NAME"),
  user: Js.Dict.unsafeGet(env, "DB_USER"),
  password: Js.Dict.unsafeGet(env, "DB_PASSWORD"),
};
let slackBotId = "DDWCM043F";