[%bs.raw {|require('./semantic/semantic.css')|}];


let s2e = ReasonReact.stringToElement;

type action =
  | Play
  | Pause
  | Stop;

module type ButtonInfo = {
    let getAction: action;
    let getColor: string;
    let getIconName: string;
    let getName: string;
};

module Button = (Info: ButtonInfo) => {
    let component = ReasonReact.statelessComponent(Info.getName ++ "Button");
    let make = (~reducer, _children) => {
        ...component,
        render: _self =>
         <button className=("ui labeled icon button " ++ Info.getColor) onClick=reducer((_event) => Info.getAction)>
            <i className=(Info.getIconName ++ " icon")/>
            (s2e(Info.getName))
         </button>
    }
};

module PlayButton = Button({
    let getAction = Play;
    let getColor = "blue";
    let getIconName = "play";
    let getName = "Play";
});

module StopButton = Button({
    let getAction = Stop;
    let getColor = "red";
    let getIconName = "stop";
    let getName = "Stop";
});

module PauseButton = Button({
    let getAction = Pause;
    let getColor = "gray";
    let getIconName = "pause";
    let getName = "Pause";
});

let pomoDuration: Luxon.Duration.t = Luxon.Duration.fromMillis(1000 * 60 * 25);

type status =
  | Ready
  | PlayingSince(Luxon.DateTime.t)
  | PausedAfter(Luxon.Duration.t);

let component = ReasonReact.reducerComponent("Execute");

let wouldBeStarting = dur =>
  Luxon.DateTime.minus(Luxon.DateTime.localNow(), dur);

let mmSs = dur => Luxon.Duration.toFormat(dur, "mm:ss");

let make = _children => {
  ...component,
  initialState: () => Ready,
  reducer: (action, state) =>
    switch action {
    | Play =>
      switch state {
      | Ready => ReasonReact.Update(PlayingSince(Luxon.DateTime.localNow()))
      | PlayingSince(_) => ReasonReact.NoUpdate
      | PausedAfter(dur) =>
        ReasonReact.Update(PlayingSince(wouldBeStarting(dur)))
      }
    | Pause =>
      switch state {
      | Ready => ReasonReact.NoUpdate
      | PlayingSince(someTime) =>
        ReasonReact.Update(
          PausedAfter(Luxon.DateTime.diff(Luxon.DateTime.localNow(), someTime))
        )
      | PausedAfter(_) => ReasonReact.NoUpdate
      }
    | Stop => ReasonReact.Update(Ready)
    },
  render: self =>
    <div>
      <p>
        (s2e("Omg hello world amirite" ++ Luxon.om))
      </p>
      (switch self.state {
        | Ready => <PlayButton reducer=self.reduce/>
        | PlayingSince(someTime) => <div>
            <PauseButton reducer=self.reduce/>
            <StopButton reducer=self.reduce/>
            <p>(s2e("Playing since: " ++ Luxon.DateTime.toString(someTime)))</p>
            </div>
        | PausedAfter(dur) => <div>
         <PlayButton reducer=self.reduce/>
          <StopButton reducer=self.reduce/>
         <p>(s2e("Paused after: " ++ mmSs(dur)))</p>
        </div>
        })
    </div>
};