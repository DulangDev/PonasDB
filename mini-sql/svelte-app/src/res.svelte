<style>
#results{
    padding: 0.5rem;
}


</style>
<script>
	export let r = [1, 2, 3, 4];
	import Comp from "./comp.svelte"
</script>
<div id = "results">
	{#each r as item}
	<Comp info={item}/>
	{/each}
</div>