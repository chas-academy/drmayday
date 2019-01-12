let helpMessage = sendMessage =>
  sendMessage(Slack.Message.helpMessage) |> ignore;

let unknownCommand = sendMessage =>
  sendMessage("Hmm, I don't know that command! Try `help` :robot_face:")
  |> ignore;