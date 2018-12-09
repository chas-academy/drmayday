module Utils = {
  let parseCommand = text =>
    text
    |> Js.String.toLowerCase
    |> Js.String.split(" ")
    |> Js.Array.slice(~start=0, ~end_=1)
    |> (array => array[0]);
};

type commands =
  | Help
  | Mayday
  | Next
  | NextForce
  | Queue
  | Unknown;

let decodeCommand = text =>
  switch (text |> Utils.parseCommand) {
  | "help" => Help
  | "mayday" => Mayday
  | "next" => Next
  | "next!" => NextForce
  | "q"
  | "queue" => Queue
  | _ => Unknown
  };