<style>
.ponas{
    display: flex;
    flex-direction: column;
    border-radius: 1rem;
    box-shadow: 0 0 5px blue;
    padding: 1rem;
	margin: 0.5rem;
}
</style>
<script>
	export let info;
</script>
<div class="ponas">
	<div>
		{info.name}
	</div>
	<div>
		balance : {info.balance}
	</div>
	<div>_id:{info._id}
	</div>
</div>