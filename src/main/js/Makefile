all: 
	cat coordinate.js move.js gamestate.js main.js > nodejs.js
	echo "#!/bin/bash\nnode nodejs.js" > ./performance
	chmod +x ./performance
clean:
	-rm nodejs.js result
