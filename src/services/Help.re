open Js.Promise;

let mayday = (text, user, isAdmin, sendMessage, sendMessageWithAttachments) =>
  isAdmin ?
    sendMessage("Hey, this is restricted to students!") |> ignore :
    user
    |> Database.hasUnfinishedHelpItem
    |> then_(hasUnfinished => {
         hasUnfinished ?
           sendMessage("You're already on the help list!") |> ignore :
           Database.addHelpItem(user, text, sendMessageWithAttachments);

         resolve();
       })
    |> ignore;

let selectRoom = (selectedOption, itemId, sendMessage) =>
  switch (selectedOption) {
  | Some(room) =>
    Database.addRoom(int_of_string(itemId), List.hd(room))
    |> then_(roomWasAdded => {
         roomWasAdded ?
           Database.getQueuePosition(itemId)
           |> then_(queuePosition => {
                let msg =
                  switch (queuePosition) {
                  | Some(queueNum) =>
                    "Ok, have a seat and relax. You’re number "
                    ++ string_of_int(queueNum)
                    ++ " in line. :coffee:"
                  | None => "Oops. Couldn't get your position in queue."
                  };

                sendMessage(msg) |> ignore;

                resolve();
              })
           |> ignore :
           sendMessage("Something went wrong. Try again later!") |> ignore;
         resolve();
       })
    |> ignore
  | None => ()
  };