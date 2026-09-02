# Changelogs 

# 10/08/26 
1. Fix del highway minigame (guitar hero) delle note che vengono registrate come missing quando lo slider non è manco partito.
   (se notate che fa un hit in ritardo o prima , dovete andare a tunare la variabile **Hit Tolerance** con un valore compreso tra 20 e non piu di 60-70)
   
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


# 20/08/26
1. Fix del crash in retry - ho provato una decina di volte e è andato tutto liscio quindi molto probabilmente si puo chiudere.
2. Aggiunte le collisioni alle parti vuote in MaxHouse, la blueprint si chiama *'BP_BlankCollision'* che trovate in **'/Content/ICC/BluePrints/Actors/Utils'** e puo essere usata come collisione vuota su tutte le mappe che hanno voragini , potete cambiare estensione della box collision come volete.

-- mi sono preso qualche giorno di riposo --

# 31/08/26
1. Ho rivisto i valori di ap modifiers e mi ero accorto che avevo fatto un blind copy-paste con i valori tutti a 0 ovviamente
   ha sminchiato il sistema di ap, dovrebbe adesso funzionare correttamente
2. Seguendo il punto 1 ho fatto un altra tab sul debugger (vi ricordo che dovete premere C e poi Ctr + C per avere il focus e cambiare tabs e poi ripremere da capo la stessa combinazione per ridare il focus a unreal) chiamata Ap flows che mostra in tempo reale current Ap e ap modifiers
3. Fixato anche anger e in generale qualsiasi buff atk di ogni emozione , adesso prende correttamente il parametro buffato. 

> **_NOTA:_**: **non seguite troppo fedelmente i log perche vengono chiamati in update e spesso potrebbero anche stampare cose non loro , usate il debugger come source of truth che quello non mente**


# 1/09/26
1. Fix del crash di buff atk , ho controllato buff def e quello funziona perche avevo messo il nuovo controllo ma mi ero scordato a metterlo nel buff atk. 
2. Fix del animazione run che si avvia anche da fermo premendo shift 


# 2/09/26
Aggiunti i seguenti parametri al AngerAtkMinigame:
- **SmashRate**: sarebbe l'impulso con cui smasha e scende subito da non confondere con decreaseSpeed che è la velocita di decremento. 
- **IncreaseSpeed**: nome abbastanza esplicativo. 

Questa implementata in data odierna consideratela una falsa alpha perche di base il funzionamento è un button smasher ma va tunato da voi pero se c'è qualcosa che non va scrivetemelo in board. 
Vi spiego come funzionano i settaggi:

- *IncreaseSpeed* deve essere un valore che mediamente **deve** oscillare tra 0.5 e **mai** sopra il **2.0**
- *DecreaseSpeed* deve essere un valore molto inferiore al Increase perche se mettete li stessi valori o mettete un valore piu alto di IncreaseSpeed si annullano a vicenda quindi il range ideale sarebbe tra ** 0.1 e non oltre 1.1 **
- *SmashRate* questo è piu particolare , è impulso dello smash , questo è un valore che non deve superare **categoricamente 1.0**


# Bugs Gravi
1. ~~Il room toggler che fa falso contatto se si corre~~ -corretto da voi tunando la walk speed e max speed-
2. Se il retry button sostanzialmente funziona solo a me è un problema e rischia di rompere tutto

# Bugs Intermedi
1. durante la selezione di EV il target nel display rimane enemy corrente e questo mi da fastidio oltre a essere molto 'misleading' pero il target rimangono corretti.

# Bugs Minori
1. Nel curling minigame manca ancora il posizionamento della palla in maniera speculare se si tocca con la barra la parte alta degli scaglioni

2. Lo scroll alcune volte non si aggiorna bene e ha bisogno di un altro ciclo di scroll per aggiornarsi correttamente

3. Joy impiega qualche microsecondo tra fine minigame e attacco per ritornare in posizione 

