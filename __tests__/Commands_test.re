open Jest;

describe("#Slack.Commands", () =>
  Expect.(
    describe("decodeCommand", () => {
      test("clear", () =>
        expect(Commands.decodeCommand("clear")) |> toEqual(Commands.Clear)
      );

      test("help", () =>
        expect(Commands.decodeCommand("help")) |> toEqual(Commands.Help)
      );

      test("m", () =>
        expect(Commands.decodeCommand("m")) |> toEqual(Commands.Mayday)
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

      test("remove", () => 
        expect(Commands.decodeCommand("remove")) |> toEqual(Commands.Remove)
      );
    })
  )
);