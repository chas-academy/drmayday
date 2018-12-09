open Jest;

describe("#Slack.Commands", () =>
  Expect.(
    describe("decodeCommand", () => {
      test("help", () =>
        expect(Commands.decodeCommand("help")) |> toEqual(Commands.Help)
      );

      test("mayday", () =>
        expect(Commands.decodeCommand("mayday")) |> toEqual(Commands.Mayday)
      );

      test("next", () =>
        expect(Commands.decodeCommand("next")) |> toEqual(Commands.Next)
      );

      test("next!", () =>
        expect(Commands.decodeCommand("next!"))
        |> toEqual(Commands.NextForce)
      );

      test("q", () =>
        expect(Commands.decodeCommand("q")) |> toEqual(Commands.Queue)
      );

      test("queue", () =>
        expect(Commands.decodeCommand("queue")) |> toEqual(Commands.Queue)
      );
    })
  )
);