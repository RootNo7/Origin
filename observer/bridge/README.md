# Bridge Protocol

`state.txt` is the current simulation snapshot. `terrain.txt` is a separate cached terrain snapshot keyed by `terrain_revision`, so resource-only changes do not rewrite the terrain.

Both published snapshots use temporary files followed by rename. External command writers should also publish `commands.txt` atomically.

## Action channel

```text
gather <actor_id> <resource_id> <amount>
```

The simulation validates actor identity, life state, target existence, amount, inventory capacity and a 2.25 m range. Results are written as:

```text
accepted 2 stone
rejected 0 out_of_range
```
