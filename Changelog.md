# Changelogs 

# 10/08/26 
1. Fix del highway minigame (guitar hero) delle note che vengono registrate come missing quando lo slider non è manco partito.
2. Fix del button smashing. Se si preme spazio si riempie se si rilascia si svuota, i valori di velocita di riempimento e svuotamento si possono tunare dentro la blueprint stessa. 
3. Fix Del curling, il minigame ora va , registra correttamente gli scaglioni e la palla si muove e si ferma dove dovrebbe tranne che per la parte destra.
4. Fix del player che non la smette piu di camminare, anche qua se si premeva shift e si rilasciava la speed non veniva aggiornata.


# 12/08/26
1. Aggiunta delle label 'Miss' e 'Nice' al highway minigame, i valori di offsetX e offsetY delle label possono essere tunati dentro la blueprint.
2. Fix del Hallway traveler: Il traveler adesso avrà un nuovo flag chiamato **'RememberCounter'** che se selezionato andrà a fare la transizione da *A - B* a *B - A*. Esempio il traveler del soggiorno di MaxHouse se si va in una stanza e si ritorna prima la transizione era sempre *A - B* quindi te ritornavi e si metta sul corridoio. Ora con sta flag si ricorda della transizione passata e quindi farà *B - A*.

# 16/08/26
1. Scoperto il motivo per il quale alcune volte il player gli manca la stanza sotto ai piedi e cade nel vuoto:
  hallway traveler in determinate circostanze quando si corre o quando si va piano non teletrasporta il player correttamente restando praticamente fermo al trigger box d'entrata.

2. Fix di alcuni bug grafici riguardanti le animazioni di Veyl.

3. Fix del combat loop animation di Veyl

4. Ho scoperto un bug minore sulla selezione degli EV , se si spara un EV a display viene indicato il nome della blueprint del Player invece che di Veyl e sopratutto se si scrolla si fissa sul primo mob in ordine pero il target rimane correttamente il Player.

# 18/08/26

1. Aggiunta del run alla blueprint animation

2. Ho sistemato da capo il room handler di MaxHouse , ho provato 6 volte e adesso le stanze correttamente si accendono e spengono, rimane sempre il problema che se corri non si teletrasporta ma ancora non ho un fix per questo in caso non lo facciamo correre durante quelle scene.

3. Sistemato a livello di display testuale nel computer di battaglia i vari target aggiustando il player perche prima compariva solo le emozioni ora a seconda del target si aggiorna correttamente


# Bugs Gravi
1. Il room toggler che fa falso contatto se si corre  

# Bugs Intermedi
Nessuno

# Bugs Minori

1. Lo scroll alcune volte non si aggiorna bene e ha bisogno di un altro ciclo di scroll per aggiornarsi correttamente
2. Joy impiega qualche microsecondo tra fine minigame e attacco per ritornare in posizione 

