let handleEvent = event => {
  let {user, text, command, channel, subtype}: Slack.Event.event = event;
  Js.Promise.(
    Slack.IO.isAdmin(user)
    |> then_(isAdmin => {
         let sendMessage = Slack.Message.send(channel);
         let sendMessageWithAttachments =
           Slack.Message.sendWithAttachments(channel);

         switch (subtype) {
         | Human =>
           switch (command) {
           | Help =>
             Slack.Message.send(channel, Slack.Message.helpMessage) |> ignore
           | Mayday =>
             isAdmin ?
               Slack.Message.send(
                 channel,
                 "Hey, this is restricted to students!",
               )
               |> ignore :
               Database.addHelpItem(user, text, sendMessage) |> ignore
           | Next =>
             isAdmin ?
               Database.getFirstHelpItem(
                 sendMessage,
                 sendMessageWithAttachments,
               )
               |> ignore :
               Slack.Message.send(
                 channel,
                 "Hey, this is restricted to teachers!",
               )
               |> ignore
           | Queue => Database.getOpenItems(sendMessage) |> ignore
           | _ => Slack.Message.send(channel, "Unknown command..") |> ignore
           }
         | Bot => ()
         };
         resolve();
       })
    |> ignore
  );
};