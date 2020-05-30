<script>
	import Results from "./res.svelte"
    let type = undefined;
    let query= "";
	let result = [];
	function get_results(){
		fetch(`/query?${type} ${query}`).then( r=> r.json() ).then((r)=>{
            result = r;
        })
	}
</script>
<style>
	.searchstr{
    display: flex;
    flex-direction: row;
    border-radius: 0.5rem;
    background: cyan;
    padding: 0.25rem;
    align-items: center;
    justify-content: center;
}

select{
    padding: 0.5rem;
    background: white;
    border:none;
}

.searchstr > input {
    border:none;
    flex: 1 0 auto;
    border-radius: 0.5rem;
    padding: 0.5rem;
}

.searchstr > div{
    padding: 0.5rem;
}


</style>
<div class="searchstr">
        <select bind:value={type}>
            <option>select</option>
            <option>insert</option>
            <option>update</option>
        </select>
        <input id="inp" bind:value={query}/>
        <div id="search" on:click={get_results}>{type}</div>
</div>
    <Results r={result}/>
    

