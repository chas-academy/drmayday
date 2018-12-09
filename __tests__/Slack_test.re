open Jest;

describe("#Slack.Event", () => {
  open Expect;

  describe("parseQuery", () =>
    test("handles command query", () =>
      expect(Slack.Event.parseQuery("mayday test")) |> toEqual("test")
    )
  );

  describe("eventPayload", () =>
    test("parses eventPayload", () => {
      let mockEvent: Slack.Event.eventPayload = {
        event:
          Some({
            subtype: Human,
            user: "UXXXXXXX",
            command: Commands.Mayday,
            channel: "AA12345678",
            text: "helpDescription",
          }),
        eventType: EventCallback,
      };
      let mockPayload =
        {|
          {
            "token": "abcdefghijkl",
            "team_id": "T4123123",
            "api_app_id": "APIAPPID",
            "event": {
              "type": "message",
              "user": "UXXXXXXX",
              "text": "mayday helpDescription",
              "client_msg_id": "9114d6eb-ae50-4ded-8151-df484d3e6884",
              "ts": "1544370473.003100",
              "channel": "AA12345678",
              "event_ts": "1544370473.003100",
              "channel_type": "im"
            },
            "type": "event_callback",
            "event_id": "EvEP58J3DZ",
            "event_time": 1544370473,
            "authed_users": ["UXXXXXXX"]
          }
        |}
        |> Json.parseOrRaise;

      let {eventType, event}: Slack.Event.eventPayload =
        mockPayload |> Slack.Event.decodeEventPayload;
      let {eventType: eType, event: e}: Slack.Event.eventPayload = mockEvent;

      expect((eventType, event)) |> toEqual((eType, e));
    })
  );

  describe("verification", () =>
    test("parses verification", () => {
      let mockBody =
        {|
        {
          "token": "Jhj5dZrVaK7ZwHHjRyZWjbDl",
          "challenge": "3eZbrw1aBm2rZgRNFdxV2595E9CY3gmdALWMmHkvFXO7tYXAYM8P",
          "type": "url_verification"
        }
      |}
        |> Json.parseOrRaise;

      let expected = "3eZbrw1aBm2rZgRNFdxV2595E9CY3gmdALWMmHkvFXO7tYXAYM8P";

      expect(mockBody |> Slack.Verification.challengeToken)
      |> toEqual(expected);
    })
  );

  describe("#userId", () =>
    test("should a parsed user id", () =>
      expect(Slack.Utils.encodeUserId("AB123")) |> toEqual("<@AB123>")
    )
  );
});