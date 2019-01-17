module Utils = {
  let formatTimestamp = timestamp => {
    let dateAdded = Js.Date.fromFloat(timestamp);
    let now = Js.Date.fromFloat(Js.Date.now());
    DateFns.distanceInWords(dateAdded, now) ++ " ago";
  };
};

type helpItem = {
  id: int,
  userId: string,
  description: string,
  room: option(string),
  timeCreated: float,
  timeClosed: option(float),
  finished: bool,
};

module Decode = {
  open Json.Decode;

  let helpItem = json => {
    id: json |> field("id", int),
    userId: json |> field("user_id", string),
    description: json |> field("description", string),
    room: json |> optional(field("room", string)),
    timeCreated: json |> field("time_created", Json.Decode.float),
    timeClosed: json |> optional(field("time_closed", Json.Decode.float)),
    finished:
      switch (json |> field("finished", int)) {
      | 1 => true
      | _ => false
      },
  };
};

let connect = () =>
  MySql2.Connection.connect(
    ~host=Config.db.host,
    ~port=3306,
    ~user=Config.db.user,
    ~password=Config.db.password,
    ~database=Config.db.name,
    (),
  );

let addHelpItem = (userId, description, sendMessageWithAttachments) => {
  let conn = connect();
  let timestamp = Js.Date.now();

  let params =
    MySql2.Params.named(
      Json.Encode.(
        object_([
          ("user_id", string(userId)),
          ("description", string(description)),
          ("time_created", Json.Encode.float(timestamp)),
          ("finished", bool(false)),
        ])
      ),
    );

  MySql2.execute(
    conn,
    "INSERT INTO help_items (user_id, description, time_created, finished) VALUES (:user_id, :description, :time_created, :finished)",
    Some(params),
    res => {
      switch (res) {
      | `Error(e) => Js.log2("ERROR: ", e)
      | `Select(select) => Js.log2("SELECT: ", select)
      | `Mutation(mutation) =>
        Js.log2("MUTATION: ", mutation);
        sendMessageWithAttachments(
          "OK, I've added you to the queue, but please tell me which room you're in: "
          ++ Slack.Utils.encodeUserId(userId),
          Slack.Message.specifyRoom(mutation |> MySql2.Mutation.insertId),
        )
        |> ignore;
      };

      MySql2.Connection.close(conn);
    },
  );
};

let addRoom = (itemId, room) =>
  Js.Promise.make((~resolve as _resolve, ~reject as _reject) => {
    let conn = connect();

    let params =
      MySql2.Params.named(
        Json.Encode.(
          object_([("id", int(itemId)), ("room", string(room))])
        ),
      );

    MySql2.execute(
      conn,
      "UPDATE help_items SET room = :room WHERE id = :id",
      Some(params),
      res => {
        switch (res) {
        | `Error(e) => Js.log2("ERROR: ", e)
        | `Select(select) => Js.log2("SELECT: ", select)
        | `Mutation(mutation) =>
          Js.log2("MUTATION: ", mutation);
          _resolve(. true);
        };

        MySql2.Connection.close(conn);
      },
    );
  });

let markAllAsFinished = () =>
  Js.Promise.make((~resolve, ~reject as _reject) => {
    let conn = connect();

    MySql2.execute(
      conn,
      "UPDATE help_items SET finished = true WHERE finished = false",
      None,
      res => {
        switch (res) {
        | `Error(e) => Js.log2("ERROR: ", e)
        | `Select(select) => Js.log2("SELECT:", select)
        | `Mutation(mutation) =>
          Js.log2("MUTATION: ", mutation);
          resolve(. mutation |> MySql2.Mutation.affectedRows);
        };

        MySql2.Connection.close(conn);
      },
    );
  });

let closeHelpItem = itemId =>
  Js.Promise.make((~resolve, ~reject as _reject) => {
    let conn = connect();
    let timeClosed = Js.Date.now();

    let params =
      MySql2.Params.named(
        Json.Encode.(
          object_([
            ("id", int(itemId)),
            ("time_closed", Json.Encode.float(timeClosed)),
            ("finished", bool(true)),
          ])
        ),
      );

    MySql2.execute(
      conn,
      "UPDATE help_items SET time_closed = :time_closed, finished = :finished WHERE id = :id",
      Some(params),
      res => {
        switch (res) {
        | `Error(e) => Js.log2("ERROR: ", e)
        | `Select(select) => Js.log2("SELECT: ", select)
        | `Mutation(mutation) =>
          Js.log2("MUTATION: ", mutation);
          resolve(. true);
        };

        MySql2.Connection.close(conn);
      },
    );
  });

let getFirstHelpItem = () =>
  Js.Promise.make((~resolve, ~reject as _reject) => {
    let conn = connect();

    MySql2.execute(
      conn,
      "SELECT * from help_items WHERE finished = false ORDER BY time_created ASC LIMIT 1",
      None,
      res => {
        switch (res) {
        | `Error(e) => Js.log2("ERROR: ", e)
        | `Select(select) =>
          let rows =
            select
            ->MySql2.Select.rows
            ->Belt.Array.map(item => item |> Decode.helpItem);

          switch (Belt.Array.length(rows)) {
          | 0 => resolve(. None)
          | _ => resolve(. Some(rows[0]))
          };
        | `Mutation(mutation) => Js.log2("MUTATION: ", mutation)
        };

        MySql2.Connection.close(conn);
      },
    );
  });

let getOpenItems = sendMessage => {
  let conn = connect();

  MySql2.execute(
    conn,
    "SELECT * from help_items WHERE finished = false ORDER BY time_created ASC",
    None,
    res => {
      switch (res) {
      | `Error(e) => Js.log2("ERROR: ", e)
      | `Select(select) =>
        let rows =
          select
          ->MySql2.Select.rows
          ->Belt.Array.map(item => item |> Decode.helpItem);
        (
          switch (Belt.Array.length(rows)) {
          | 0 => "Woot. No one on the help list? Nice work!"
          | _ =>
            "*Here's the current list of patients:*\n\n"
            ++ (
              rows->Belt.Array.mapWithIndex(
                (i, {userId, description, room, timeCreated}) =>
                "*"
                ++ string_of_int(i + 1)
                ++ "*. "
                ++ Slack.Utils.encodeUserId(userId)
                ++ " - "
                ++ description
                ++ " - "
                ++ Utils.formatTimestamp(timeCreated)
                ++ (
                  switch (room) {
                  | Some(r) => " in *" ++ Slack.Message.parseRoom(r) ++ "*"
                  | None => ""
                  }
                )
              )
              |> Js.Array.joinWith("\n")
            )
          }
        )
        |> sendMessage;
        ();
      | `Mutation(mutation) => Js.log2("MUTATION: ", mutation)
      };

      MySql2.Connection.close(conn);
    },
  );
};

let hasUnfinishedHelpItem = userId =>
  Js.Promise.make((~resolve, ~reject as _) => {
    let conn = connect();

    let params =
      MySql2.Params.named(
        Json.Encode.(object_([("user_id", string(userId))])),
      );

    MySql2.execute(
      conn,
      "SELECT * FROM help_items WHERE user_id = :user_id AND finished = false",
      Some(params),
      res => {
        switch (res) {
        | `Error(e) => Js.log2("ERROR: ", e)
        | `Select(select) =>
          let rows =
            select
            ->MySql2.Select.rows
            ->Belt.Array.map(item => item |> Decode.helpItem);
          switch (Array.length(rows)) {
          | 0 => resolve(. false)
          | _ => resolve(. true)
          };
        | `Mutation(mutation) => Js.log2("MUTATION: ", mutation)
        };

        MySql2.Connection.close(conn);
      },
    );
  });