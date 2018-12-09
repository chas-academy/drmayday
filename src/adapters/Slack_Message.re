let send = (_channel: string, message: string) =>
  {"channel": Config.slackBotId, "text": message, "mrkdwn": true}
  |> Slack_IO.sendPayload;

let sendWithAttachments = (channel: string, message: string, attachments) =>
  {
    "channel": channel,
    "text": message,
    "attachments": attachments,
    "mrkdwn": true,
  }
  |> Slack_IO.sendPayload;

let confirmQueueRemoval = itemId => {j|[
       {
         "fallback": "Seems like your interface does not suppor attachments.",
         "callback_id": "confirm_finished",
         "color": "#3AA3E3",
         "attachment_type": "default",
         "actions": [
           {
             "name": "remove",
             "text": "Yes",
             "type": "button",
             "value": $itemId,
           },
           {
             "name": "keep",
             "text": "No",
             "type": "button",
             "value": "no",
           },
         ],
       },
     ]
     |j};