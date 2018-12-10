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
           | Help => sendMessage(Slack.Message.helpMessage) |> ignore
           | Mayday =>
             isAdmin ?
               sendMessage("Hey, this is restricted to students!") |> ignore :
               Database.addHelpItem(user, text, sendMessageWithAttachments)
           | Next =>
             !isAdmin ?
               Database.getFirstHelpItem(
                 sendMessage,
                 sendMessageWithAttachments,
               )
               |> ignore :
               sendMessage("Hey, this is restricted to teachers!") |> ignore
           | Queue => Database.getOpenItems(sendMessage) |> ignore
           | _ => sendMessage("Unknown command..") |> ignore
           }
         | Bot => ()
         };
         resolve();
       })
    |> ignore
  );
};