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
           | Queue =>
             Queue.getOpenItems(Slack.Message.queue)
             |> then_(sendMessage)
             |> ignore
           | Remove => Queue.remove(user, isAdmin, sendMessage)
           | Room =>
             Help.updateRoom(user, isAdmin, sendMessage, sendMessageWA)
           | _ => Misc.unknownCommand(sendMessage)
           }
         | Bot => ()
         };

         resolve();
       })
    |> ignore
  );
};

module Api = {
  module Decode = {
    let helpItem =
        ({userId, description, room, timeCreated}: Database.helpItem) =>
      Json.Encode.(
        object_([
          ("user_id", string(userId)),
          ("description", string(description)),
          ("time_created", float(timeCreated)),
          (
            "room",
            string(
              Slack.Message.parseRoom(Belt.Option.getWithDefault(room, "")),
            ),
          ),
        ])
      );
  };

  module Message = {
    let queue = helpItems => Json.Encode.array(Decode.helpItem, helpItems);
  };
};

let handleApiEvent = (~user=?, event) => {
  let {args: _args, command}: Slack.Api.event = event;
  switch (command) {
  /*
   | Clear => Queue.clear(isAdmin)
   | Help => Misc.helpMessage()
   | Mayday => Help.mayday(text, user, isAdmin)
   | Next => Queue.next(isAdmin)
   | Remove => Queue.remove(user, isAdmin)
   | Room => Help.updateRoom(user, isAdmin)
   */
  | Queue => Queue.getOpenItems(Api.Message.queue)
  };
};
