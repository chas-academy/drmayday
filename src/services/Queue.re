open Js.Promise;

let clear = (isAdmin, sendMessage) =>
  isAdmin ?
    Database.markAllAsFinished()
    |> then_(affectedRows => {
         let message =
           switch (affectedRows) {
           | 0 => "Nothing to clear!"
           | _ => "OK. Marked all items as finished! Hope you know what you're doing! :grimacing"
           };

         sendMessage(message);
       })
    |> ignore :
    sendMessage("Nice try! :face_with_hand_over_mouth:") |> ignore;

let next = (isAdmin, sendMessage, sendMessageWithAttachments) =>
  isAdmin ?
    Database.getFirstHelpItem()
    |> then_(item => {
         switch (item) {
         | Some((helpItem: Database.helpItem)) =>
           sendMessageWithAttachments(
             "Really remove "
             ++ Slack.Utils.encodeUserId(helpItem.userId)
             ++ " from the queue? Have they received help?",
             Slack.Message.confirmQueueRemoval(helpItem.id),
           )
           |> ignore
         | None =>
           "There's no one next in line! Nice work! :tada:"
           |> sendMessage
           |> ignore
         };
         resolve();
       })
    |> ignore :
    sendMessage("Hey, this is restricted to teachers! :face_with_monocle:")
    |> ignore;

let getOpenItems = sendMessage =>
  Database.getOpenItems()
  |> then_((helpItems: array(Database.helpItem)) => {
       let message =
         switch (Belt.Array.length(helpItems)) {
         | 0 => "Woot. No one on the help list? Nice work!"
         | _ =>
           "*Here's the current list of patients:*\n\n"
           ++ (
             helpItems->Belt.Array.mapWithIndex(
               (i, {userId, description, room, timeCreated}) =>
               "*"
               ++ string_of_int(i + 1)
               ++ "*. "
               ++ Slack.Utils.encodeUserId(userId)
               ++ " - "
               ++ description
               ++ " - "
               ++ Database.Utils.formatTimestamp(timeCreated)
               ++ (
                 switch (room) {
                 | Some(r) => " in *" ++ Slack.Message.parseRoom(r) ++ "*"
                 | None => ""
                 }
               )
             )
             |> Js.Array.joinWith("\n")
           )
         };

       sendMessage(message);
       resolve();
     })
  |> ignore;

let markAsFinished = (itemId, sendMessage) =>
  switch (itemId) {
  | Some(id) =>
    Database.closeHelpItem(int_of_string(id))
    |> then_(_del => {
         Database.getFirstHelpItem()
         |> then_(item => {
              switch (item) {
              | Some((helpItem: Database.helpItem)) =>
                sendMessage(
                  "Ok, "
                  ++ Slack.Utils.encodeUserId(helpItem.userId)
                  ++ " is next in line. The topic is: "
                  ++ helpItem.description
                  ++ "\nThey're "
                  ++ (
                    switch (helpItem.room) {
                    | Some(r) =>
                      "in " ++ Slack.Message.parseRoom(r) ++ " :runner:"
                    | None => " somewhere. :thinking_face:"
                    }
                  ),
                )
                |> ignore
              | None =>
                "No one next in line! Nice work! :clap:"
                |> sendMessage
                |> ignore
              };
              resolve();
            })
         |> ignore;

         resolve(true);
       })
    |> ignore
  | None => ()
  };

let remove = (user, isAdmin, sendMessage) => {
  isAdmin ?
    sendMessage("Hey, this is restricted to students!") |> ignore :
    user
    |> Database.hasUnfinishedHelpItem
    |> then_(((hasUnfinished, itemId)) => {
         !hasUnfinished ?
           sendMessage(
             "You don't have an active help-ticket. Nothing to clear!",
           )
           |> ignore :
           (
             switch (itemId) {
             | None => ()
             | Some(id) =>
               id
               |> Database.closeHelpItem
               |> then_(_closed =>
                    sendMessage("OK, I've closed your ticket!")
                  )
               |> ignore
             }
           );

         resolve();
       })
    |> ignore;
};