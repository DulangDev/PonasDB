let editor = document.getElementById("inp");
let btn = document.getElementById("search");
let res_container = document.getElementById("results");

function ponasenkov(info){
    console.log(info);
    return `<div class="ponas">
        <div>${info.name}</div>
        <div>tel: ${info.num}</div>
    </div>`;
}


btn.onclick = ()=>{
    fetch("/?"+editor.value).then( r=> r.json() ).then(
        (r)=>{

            res_container.innerHTML = r.map(ponasenkov).reduce((a, v)=> a + v, "");
        }
    )
}
