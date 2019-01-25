module Utils = {
  let parseCommand = text =>
    text
    |> Js.String.toLowerCase
    |> Js.String.split(" ")
    |> Js.Array.slice(~start=0, ~end_=1)
    |> (array => array[0]);
};

type commands =
  | Clear
  | Help
  | Mayday
  | Next
  | NextForce
  | Queue
  | Remove
  | Room
  | Unknown;

let decodeCommand = text =>
  switch (text |> Utils.parseCommand) {
  | "clear" => Clear
  | "help" => Help
  | "m"
  | "mayday" => Mayday
  | "next" => Next
  | "next!" => NextForce
  | "q"
  | "queue" => Queue
  | "remove" => Remove
  | "room" => Room
  | _ => Unknown
  };