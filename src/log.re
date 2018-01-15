[%bs.raw {|require('./semantic/semantic.css')|}];

type action = Nada;

type entry = {
    estimated: int,
    soFar: int,
    completed: bool
  };
  
  module StringMap =
    Map.Make(
      {
        type t = string;
        let compare = (x1, x2) => String.compare(x1, x2);
      }
    );
  
let component = ReasonReact.reducerComponent("Log");

let s2e = ReasonReact.stringToElement;

let make = _children => {
    ...component, 
    initialState: () => StringMap.(empty 
    |> add("make notifications work", {estimated: 2, soFar: 2, completed:true})
    |> add("implement the log", {estimated: 2, soFar:1, completed: false})
    ),
    reducer: (action, state) => switch action {
    | Nada => (StringMap.is_empty(state) ? ReasonReact.NoUpdate : ReasonReact.NoUpdate);
    },
    render: self => {
        <div className="logTable">
        <table className="ui celled table">
        <thead>
        <tr>
        <th>(s2e("Task Name"))</th>
        <th>(s2e("Estimated"))</th>
        <th>(s2e("So Far"))</th>
        <th>(s2e("Done?"))</th>
        </tr>
        </thead>
        { let children = StringMap.bindings(self.state) |> 
            List.stable_sort(((_, e1), (_, e2)) => compare(e1.completed, e2.completed ))
            |> List.map(
            ((n,e)) => {
                <tr className=(e.completed ? "disabled" : "")>
                    <td>(s2e(n))</td>
                    <td>(s2e(string_of_int(e.estimated)))</td>
                    <td>(s2e(string_of_int(e.soFar)))</td>
                    <td>(s2e(e.completed ? "Yep" : "Not Yet!"))</td>
                </tr>})
            |> Array.of_list;
            
            ReasonReact.createDomElement("tbody", ~props=Js.Obj.empty(), children)}
        </table>
        </div>
    }
};