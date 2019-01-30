open Js.Promise;

let mayday = (text, user, isAdmin, sendMessage, sendMessageWithAttachments) => {
  isAdmin ?
    sendMessage("Hey, this is restricted to students!") |> ignore :
    Database.hasUnfinishedHelpItem(user)
    |> then_(((hasUnfinished, _itemId)) => {
         if (hasUnfinished) {
           sendMessage("You're already on the help list!") |> ignore;
         } else {
           Database.addHelpItem(user, text)
           |> then_(itemId =>
                sendMessageWithAttachments(
                  "OK, I've added you to the queue, but please tell me which room you're in: "
                  ++ Slack.Utils.encodeUserId(user),
                  Slack.Message.specifyRoom(itemId),
                )
              )
           |> ignore;
         };

         resolve();
       })
    |> ignore;
};

let selectRoom = (selectedOption, itemId, sendMessage) =>
  switch (selectedOption) {
  | Some(room) =>
    Database.addRoom(int_of_string(itemId), List.hd(room))
    |> then_(roomWasAdded => {
         roomWasAdded ?
           Database.getQueuePosition(itemId)
           |> then_(queuePosition => {
                let message =
                  switch (queuePosition) {
                  | Some(queueNum) =>
                    "Ok, have a seat and relax. You're number "
                    ++ string_of_int(queueNum)
                    ++ " in line. :coffee:"
                  | None => "Oops. Couldn't get your position in queue."
                  };

                sendMessage(message);

                resolve(queuePosition);
              })
           |> ignore :
           sendMessage(
             "I ran into some trouble adding your location. Try again later!",
           )
           |> ignore;
         resolve();
       })
    |> ignore
  | None => ()
  };

let updateRoom = (user, isAdmin, sendMessage, sendMessageWithAttachments) => {
  isAdmin ?
    sendMessage("Hey, this is restricted to students!") |> ignore :
    Database.hasUnfinishedHelpItem(user)
    |> then_(((hasUnfinished, itemId)) => {
         hasUnfinished ?
           sendMessageWithAttachments(
             "OK, please tell me which room you're in: ",
             Slack.Message.specifyRoom(itemId),
           ) :
           sendMessage(
             "Hmm, you don't seem to have an active ticket. Use `mayday` to add yourself.",
           );

         resolve();
       })
    |> ignore;
};