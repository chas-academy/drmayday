let next = (isAdmin, sendMessage, sendMessageWithAttachments) => {
  isAdmin ?
    {
      Js.Promise.(
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
        |> ignore
      );
    } :
    sendMessage("Hey, this is restricted to teachers! :face_with_monocle:")
    |> ignore;
};

let getOpenItems = sendMessage =>
  Database.getOpenItems(sendMessage) |> ignore;

let markAsFinished = itemId => {
  switch (itemId) {
  | Some(id) => Database.closeHelpItem(int_of_string(id))
  | None => ()
  };
};