let handle = event => {
  let {user, text, command, channel, subtype}: Decode.Event.t = event;

  let sendMessage = Slack_Message.send(channel);
  let sendMessageWithAttachments = Slack_Message.sendWithAttachments(channel);

  switch (subtype) {
  | Human =>
    switch (command) {
    | Commands.Help =>
      Slack_Message.send(channel, Utils.helpMessage) |> ignore
    | Commands.Mayday =>
      Database.addHelpItem(user, text, sendMessage) |> ignore
    | Commands.Next =>
      Database.getFirstHelpItem(sendMessage, sendMessageWithAttachments)
      |> ignore
    | Commands.Queue => Database.getOpenItems(sendMessage) |> ignore
    | _ => Slack_Message.send(channel, "Unknown command..") |> ignore
    }
  | Decode.SubType.Bot => ()
  };
};