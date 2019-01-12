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
               user
               |> Database.hasUnfinishedHelpItem
               |> then_(hasUnfinished => {
                    hasUnfinished ?
                      sendMessage("You're already on the help list!")
                      |> ignore :
                      Database.addHelpItem(
                        user,
                        text,
                        sendMessageWithAttachments,
                      );

                    resolve();
                  })
               |> ignore
           | Next =>
             !isAdmin ?
               Database.getFirstHelpItem()
               |> then_(item => {
                    switch (item) {
                    | Some((helpItem: Database.helpItem)) =>
                      sendMessageWithAttachments(
                        "Remove "
                        ++ Slack.Utils.encodeUserId(helpItem.userId)
                        ++ " from the queue?",
                        Slack.Message.confirmQueueRemoval(helpItem.id),
                      )
                      |> ignore
                    | None =>
                      "There's no one next in line! Nice work!"
                      |> sendMessage
                      |> ignore
                    };
                    resolve();
                  })
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