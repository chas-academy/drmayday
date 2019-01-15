let handleEvent = event => {
  let {user, text, command, channel, subtype}: Slack.Event.event = event;
  Js.Promise.(
    Slack.IO.isAdmin(user)
    |> then_(isAdmin => {
         let sendMessage = Slack.Message.send(channel);
         let sendMessageWA = Slack.Message.sendWithAttachments(channel);

         switch (subtype) {
         | Human =>
           switch (command) {
           | Clear => Queue.clear(isAdmin, sendMessage)
           | Help => Misc.helpMessage(sendMessage)
           | Mayday =>
             Help.mayday(text, user, isAdmin, sendMessage, sendMessageWA)
           | Next => Queue.next(isAdmin, sendMessage, sendMessageWA)
           | Queue => Queue.getOpenItems(sendMessage)
           | _ => Misc.unknownCommand(sendMessage)
           }
         | Bot => ()
         };

         resolve();
       })
    |> ignore
  );
};