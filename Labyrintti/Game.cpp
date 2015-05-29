#include "Game.h"
#include "debug.hh"
#include "vaittama.hh"

Game::Game() :
mGameMode(false), mScreen(nullptr), mAreaSize(0), mActivePlayer(0)
{}


Game::~Game()
{
	DEBUG_OUTPUT("Game destructor" << std::endl);
	if (debug_output)
		system("PAUSE");
}
bool Game::onkoAlustustilassa() const{
	return !mGameMode;
}
void Game::lisaaNaytto(Julkinen::Nayttorajapinta* naytto){
	DEBUG_OUTPUT("lisaaNaytto()" << std::endl);

	// Perform checks
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, onkoAlustustilassa(), "onkoAlustustilassa", "Game.cpp", 23, "lisaaNaytto");

	// Add the screen
	mScreen = dynamic_cast<Naytto*>(naytto);
	mScreen->komentoAloitaRakennus();
}
void Game::maaritaPelialueenKoko(Julkinen::Koordinaatti const& koko){
	// Perform checks
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, mScreen != nullptr, "mScreen != nullptr", "Game.cpp", 31, "maaritaPelialueenKoko");

	// Define game area size
	mAreaSize = koko.haeXkoordinaatti();
}
void Game::lisaaPelaaja(Julkinen::PelaajaTyyppi tyyppi, std::string const& nimi, char lyhenne, Julkinen::Koordinaatti const& sijainti){
	DEBUG_OUTPUT("lisaaPelaaja()" << std::endl);

	// Perform checks
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, onkoAlustustilassa(), "onkoAlustustilassa", "Game.cpp", 36, "lisaaPelaaja");
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, mAreaSize > 0, "mAreaSize > 0", "Game.cpp", 37, "lisaaPelaaja");

	try{
		if (!sijainti.onkoIrtopala()){
			if ((int)sijainti.haeXkoordinaatti() < 1 || (int)sijainti.haeXkoordinaatti() > mAreaSize || (int)sijainti.haeYkoordinaatti() < 1 || (int)sijainti.haeXkoordinaatti() > mAreaSize)
				throw Julkinen::Alustusvirhe(Julkinen::Alustusvirhe::VIRHEELLINEN_SIJAINTI);
		}
		std::vector<Player>::iterator player = mPlayers.end();
		player = std::find_if(mPlayers.begin(), mPlayers.end(), [&sijainti](Player p){
			return sijainti == p.getLocation();
		});
		if (player != mPlayers.end())
			throw Julkinen::Alustusvirhe(Julkinen::Alustusvirhe("There is already a player on the given coordinate."));
	}
	catch (Julkinen::Alustusvirhe& av){
		av.tulosta(std::cout);
		std::cout << std::endl;
	}

	// Add the piece
	mPlayers.push_back(Player(tyyppi, nimi, lyhenne, sijainti));
}
void Game::lisaaPala(Julkinen::PalaTyyppi pala, unsigned int rotaatio, Julkinen::Koordinaatti const& sijainti){
	DEBUG_OUTPUT("lisaaPala()" << std::endl);
	// Perform checks
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, onkoAlustustilassa(), "onkoAlustustilassa", "Game.cpp", 50, "lisaaPala");
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, mAreaSize > 0, "mAreaSize > 0", "Game.cpp", 51, "lisaaPala");
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, freePieceSlot(sijainti), "freePieceSlot", "Game.cpp", 52, "lisaaPala");

	try{
		if (!sijainti.onkoIrtopala()){
			if ((int)sijainti.haeXkoordinaatti() < 1 || (int)sijainti.haeXkoordinaatti() > mAreaSize || (int)sijainti.haeYkoordinaatti() < 1 || (int)sijainti.haeXkoordinaatti() > mAreaSize)
				throw Julkinen::Alustusvirhe(Julkinen::Alustusvirhe::VIRHEELLINEN_SIJAINTI);
		}
		if (rotaatio < 1 || rotaatio > 4)
			throw Julkinen::Alustusvirhe(Julkinen::Alustusvirhe::VIRHEELLINEN_ROTAATIO);
	}
	catch (Julkinen::Alustusvirhe& av){
		av.tulosta(std::cout);
		std::cout << std::endl;
	}


	// Add the piece
	mPieces.push_back(Piece(sijainti, pala, rotaatio));
}
void Game::lisaaEsine(char merkki, Julkinen::Koordinaatti const& sijainti, std::string const& pelaaja){
	DEBUG_OUTPUT("lisaaEsine()" << std::endl);

	// Perform checks
	try{
		std::vector<Player>::iterator player = mPlayers.end();
		player = std::find_if(mPlayers.begin(), mPlayers.end(), [&pelaaja](Player p){
			return p.getName() == pelaaja;
		});
		if (player == mPlayers.end())
			throw Julkinen::Alustusvirhe(Julkinen::Alustusvirhe("Player does not exist."));

		if (!sijainti.onkoIrtopala()){
			if ((int)sijainti.haeXkoordinaatti() < 1 || (int)sijainti.haeXkoordinaatti() > mAreaSize || (int)sijainti.haeYkoordinaatti() < 1 || (int)sijainti.haeXkoordinaatti() > mAreaSize)
				throw Julkinen::Alustusvirhe(Julkinen::Alustusvirhe::VIRHEELLINEN_SIJAINTI);
		}
	}
	catch (Julkinen::Alustusvirhe& av){
		av.tulosta(std::cout);
		std::cout << std::endl;
	}


	// Find piece where this item belongs and add it
	for (int i = 0; i < (int)mPieces.size(); i++){
		if (mPieces.at(i).getLocation() == sijainti)
			mPieces.at(i).setItem(merkki);
	}

	// Find player whose target this item is and add it
	for (int i = 0; i < (int)mPlayers.size(); i++){
		if (mPlayers.at(i).getName() == pelaaja){
			mPlayers.at(i).addTargetItem(merkki);
		}
	}
}
void Game::asetaPalanTyyppi(Julkinen::ErikoispalaTyyppi tyyppi, Julkinen::Koordinaatti const& sijainti, Julkinen::Koordinaatti const& kohde){
	DEBUG_OUTPUT("asetaPalanTyyppi()" << std::endl);

	// Perform checks
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, onkoAlustustilassa(), "onkoAlustustilassa", "Game.cpp", 57, "asetaPalanTyyppi");

	try{
		if (!sijainti.onkoIrtopala()){
			if ((int)sijainti.haeXkoordinaatti() < 1 || (int)sijainti.haeXkoordinaatti() > mAreaSize || (int)sijainti.haeYkoordinaatti() < 1 || (int)sijainti.haeXkoordinaatti() > mAreaSize)
				throw Julkinen::Alustusvirhe(Julkinen::Alustusvirhe::VIRHEELLINEN_SIJAINTI);
		}
		if (!kohde.onkoIrtopala()){
			if ((int)kohde.haeXkoordinaatti() < 1 || (int)kohde.haeXkoordinaatti() > mAreaSize || (int)kohde.haeYkoordinaatti() < 1 || (int)kohde.haeXkoordinaatti() > mAreaSize)
				throw Julkinen::Alustusvirhe(Julkinen::Alustusvirhe::VIRHEELLINEN_SIJAINTI);
		}
	}
	catch (Julkinen::Alustusvirhe& av)
	{
		av.tulosta(std::cout);
		std::cout << std::endl;
	}


	std::vector<Piece>::iterator currentPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
		return piece.getLocation() == sijainti;
	});
	currentPiece->setSpecialType(tyyppi);
	currentPiece->setTarget(kohde);
}
void Game::alustusLopeta(){
	DEBUG_OUTPUT("alustusLopeta()" << std::endl);

	updateScreen();
	mScreen->komentoLopetaRakennus();
	mGameMode = true;
	mScreen->ilmoitusVuorossa(mPlayers.at(mActivePlayer).getName());
}
bool Game::onkoPelitilassa() const{
	return mGameMode;
}
void Game::komentoTyonna(Julkinen::Reuna reuna, unsigned int paikka, unsigned int rotaatio){
	DEBUG_OUTPUT("komentoTyonna()" << std::endl);

	// Perform checks
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, onkoPelitilassa(), "onkoPelitilassa", "Game.cpp", 80, "komentoTyonna");

	try{
		if (mPlayerActionStatus.isPushed()){
			throw Julkinen::Toimintovirhe(Julkinen::Toimintovirhe::VIRHE_IRTOPALAA_ON_JO_TYONNETTY);
		}

		if ((int)paikka < 1 || (int)paikka > mAreaSize){
			throw Julkinen::Komentovirhe(Julkinen::Komentovirhe::VIRHE_OLEMATON_PAIKKA);
		}
		if (rotaatio < 1 || rotaatio > 4){
			throw Julkinen::Komentovirhe(Julkinen::Komentovirhe::VIRHE_VIRHEELLINEN_ROTAATIO);
		}
	}
	catch (Julkinen::Toimintovirhe& tv){
		tv.tulosta(std::cout);
		std::cout << std::endl;
		mScreen->ilmoitusVuorossa(mPlayers.at(mActivePlayer).getName());
		return;
	}
	catch (Julkinen::Komentovirhe& kv){
		kv.tulosta(std::cout);
		return;
	}



	// Begin construction mode
	mScreen->komentoAloitaRakennus();
	mGameMode = false;

	// Get iterator to piece of opposite edge
	int x, y;
	switch (reuna){
	case Julkinen::ALA:
		y = 1;
		x = paikka;
		break;
	case Julkinen::YLA:
		y = mAreaSize;
		x = paikka;
		break;
	case Julkinen::OIKEA:
		x = 1;
		y = paikka;
		break;
	case Julkinen::VASEN:
		x = mAreaSize;
		y = paikka;
		break;
	}
	std::vector<Piece>::iterator detachPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
		return piece.getLocation() == Julkinen::Koordinaatti(x, y);
	});

	// Move pieces & players
	std::vector<Piece>::iterator movePiece;
	std::vector<Player>::iterator movePlayer = mPlayers.end();
	switch (reuna){
	case Julkinen::ALA:
		for (y += 1; y < mAreaSize + 1; y++){
			movePiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
				return piece.getLocation() == Julkinen::Koordinaatti(x, y);
			});
			movePlayer = std::find_if(mPlayers.begin(), mPlayers.end(), [&](Player player){
				return player.getLocation() == Julkinen::Koordinaatti(x, y);
			});
			movePiece->setLocation(Julkinen::Koordinaatti(x, y - 1));
			if (movePlayer != mPlayers.end()){
				if (y > 1)
					movePlayer->setLocation(Julkinen::Koordinaatti(x, y - 1));
				else
					movePlayer->setLocation(Julkinen::Koordinaatti(x, mAreaSize));
			}
			movePlayer = mPlayers.end();
		}
		movePlayer = std::find_if(mPlayers.begin(), mPlayers.end(), [&](Player player){
			return player.getLocation() == Julkinen::Koordinaatti(x, 1);
		});
		if (movePlayer != mPlayers.end())
			movePlayer->setLocation(Julkinen::Koordinaatti(x, mAreaSize));
		y--;
		break;
	case Julkinen::YLA:
		for (y -= 1; y > 0; y--){
			movePiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
				return piece.getLocation() == Julkinen::Koordinaatti(x, y);
			});
			movePlayer = std::find_if(mPlayers.begin(), mPlayers.end(), [&](Player player){
				return player.getLocation() == Julkinen::Koordinaatti(x, y);
			});
			movePiece->setLocation(Julkinen::Koordinaatti(x, y + 1));
			if (movePlayer != mPlayers.end()){
				if (y < mAreaSize)
					movePlayer->setLocation(Julkinen::Koordinaatti(x, y + 1));
				else
					movePlayer->setLocation(Julkinen::Koordinaatti(x, 1));
			}
			movePlayer = mPlayers.end();
		}
		movePlayer = std::find_if(mPlayers.begin(), mPlayers.end(), [&](Player player){
			return player.getLocation() == Julkinen::Koordinaatti(x, mAreaSize);
		});
		if (movePlayer != mPlayers.end())
			movePlayer->setLocation(Julkinen::Koordinaatti(x, 1));
		y++;
		break;
	case Julkinen::OIKEA:
		for (x += 1; x < mAreaSize + 1; x++){
			movePiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
				return piece.getLocation() == Julkinen::Koordinaatti(x, y);
			});
			movePlayer = std::find_if(mPlayers.begin(), mPlayers.end(), [&](Player player){
				return player.getLocation() == Julkinen::Koordinaatti(x, y);
			});
			movePiece->setLocation(Julkinen::Koordinaatti(x - 1, y));
			if (movePlayer != mPlayers.end()){
				if (x > 1)
					movePlayer->setLocation(Julkinen::Koordinaatti(x - 1, y));
				else
					movePlayer->setLocation(Julkinen::Koordinaatti(mAreaSize, y));
			}
			movePlayer = mPlayers.end();
		}
		movePlayer = std::find_if(mPlayers.begin(), mPlayers.end(), [&](Player player){
			return player.getLocation() == Julkinen::Koordinaatti(1, y);
		});
		if (movePlayer != mPlayers.end())
			movePlayer->setLocation(Julkinen::Koordinaatti(mAreaSize, y));
		x--;
		break;
	case Julkinen::VASEN:
		for (x -= 1; x > 0; x--){
			movePiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
				return piece.getLocation() == Julkinen::Koordinaatti(x, y);
			});
			movePlayer = std::find_if(mPlayers.begin(), mPlayers.end(), [&](Player player){
				return player.getLocation() == Julkinen::Koordinaatti(x, y);
			});
			movePiece->setLocation(Julkinen::Koordinaatti(x + 1, y));
			if (movePlayer != mPlayers.end()){
				if (x < mAreaSize)
					movePlayer->setLocation(Julkinen::Koordinaatti(x + 1, y));
				else
					movePlayer->setLocation(Julkinen::Koordinaatti(1, y));
			}
			movePlayer = mPlayers.end();
		}
		movePlayer = std::find_if(mPlayers.begin(), mPlayers.end(), [&](Player player){
			return player.getLocation() == Julkinen::Koordinaatti(mAreaSize, y);
		});
		if (movePlayer != mPlayers.end())
			movePlayer->setLocation(Julkinen::Koordinaatti(1, y));
		x++;
		break;
	}

	// Move current detached piece to empty slot
	std::vector<Piece>::iterator attachPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
		return piece.getLocation() == Julkinen::Koordinaatti();
	});
	attachPiece->setLocation(Julkinen::Koordinaatti(x, y));
	attachPiece->setRotation(rotaatio);

	// Set original piece of opposite edge as new detached piece
	detachPiece->setLocation(Julkinen::Koordinaatti());

	mPlayerActionStatus.setPushed();

	updateScreen();

	mScreen->komentoLopetaRakennus();
	mScreen->ilmoitusVuorossa(mPlayers.at(mActivePlayer).getName());
	mGameMode = true;
}
void Game::komentoLiiku(Julkinen::Suunta suunta, unsigned int maara){
	DEBUG_OUTPUT("komentoLiiku()" << std::endl);

	// Perform checks
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, onkoPelitilassa(), "onkoPelitilassa", "Game.cpp", 242, "komentoLiiku");

	if (suunta == Julkinen::AUTOMAATTI){
		handleCPU();
		// CPU actions handled, no reason to continue in method
		return;
	}

	try{
		if (!mPlayerActionStatus.isPushed()){
			throw Julkinen::Toimintovirhe(Julkinen::Toimintovirhe::VIRHE_IRTOPALAA_EI_OLE_TYONNETTY);
		}
		if ((int)maara < 0){
			throw Julkinen::Komentovirhe(Julkinen::Komentovirhe::VIRHE_TUNNISTAMATON_PARAMETRI);
		}
		if (maara == 0 && suunta != Julkinen::Suunta::PAIKALLAAN){
			throw Julkinen::Komentovirhe(Julkinen::Komentovirhe::VIRHE_TUNNISTAMATON_PARAMETRI);
		}
		if (isCollision(suunta, maara)){
			throw Julkinen::Toimintovirhe(Julkinen::Toimintovirhe::VIRHE_EI_VOITU_LIIKKUA_ANNETTUA_MAARAA);
		}
	}
	catch (Julkinen::Toimintovirhe& tv){
		tv.tulosta(std::cout);
		std::cout << std::endl;
		// At the moment, if player performs Toimintovirhe, he is NOT asked for a new command.
		mPlayerActionStatus.setMoved();
		return;
	}
	catch (Julkinen::Komentovirhe& kv){
		kv.tulosta(std::cout);
		std::cout << std::endl;
		// At the moment, if player performs Toimintovirhe, he is NOT asked for a new command.
		mPlayerActionStatus.setMoved();
		return;
	}

	if (suunta == Julkinen::PAIKALLAAN){
		mPlayers.at(mActivePlayer).setLastCommand(std::string("paikallaan"));
		mPlayerActionStatus.setMoved();
		// Player does not want to move, no reason to continue in method
		return;
	}

	// If got to this point, it is safe to move player.
	movePlayer(maara, suunta);
	mPlayers.at(mActivePlayer).setLastCommand(std::string("liiku " + directionChar(suunta) + " " + std::to_string(maara)));
	mPlayerActionStatus.setMoved();

}
bool Game::vaihdaVuoro(){
	DEBUG_OUTPUT("vaihdaVuoro()" << std::endl);

	// Perform checks
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, onkoPelitilassa(), "onkoPelitilassa", "Game.cpp", 344, "vaihdaVuoro");
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, mPlayerActionStatus.actionsDone(), "mPlayerActionStatus.actionsDone()", "Game.cpp", 345, "vaihdaVuoro");

	try{
		if (!mPlayerActionStatus.isMoved()){
			throw Julkinen::Toimintovirhe(Julkinen::Toimintovirhe::VIRHE_PELAAJA_EI_OLE_LIIKKUNUT);
		}
	}
	catch (Julkinen::Toimintovirhe& tv){
		tv.tulosta(std::cout);
		std::cout << std::endl;
		return true;
	}


	// Check if someone won game
	for (int i = 0; i < (int)mPlayers.size(); i++){
		if (mPlayers.at(i).isWinner())
		{
			mScreen->tulostaVoittaja();
			system("pause");
			return false;
		}
	};

	// Do things
	mScreen->komentoAloitaRakennus();
	mGameMode = false;

	updateScreen();

	mScreen->komentoLopetaRakennus();
	mGameMode = true;

	if (mActivePlayer == mPlayers.size() - 1)
		mActivePlayer = 0;
	else
		mActivePlayer++;
	DEBUG_OUTPUT("Active Player:" << mActivePlayer << std::endl);

	mScreen->ilmoitusVuorossa(mPlayers.at(mActivePlayer).getName());
	mPlayerActionStatus.reset();
	return true;
}
Julkinen::PelaajaTyyppi Game::haeVuorossa(){
	// Perform checks
	Julkinen::vaittamaToteutus(Julkinen::ESIEHTOVAITTAMA, onkoPelitilassa(), "onkoPelitilassa", "Game.cpp", 314, "haeVuorossa");

	return mPlayers.at(mActivePlayer).getType();
}

void Game::updateScreen(){

	std::for_each(mPieces.begin(), mPieces.end(), [&](Piece piece){
		mScreen->palaLaudalle(piece.getType(), Julkinen::ErikoispalaTyyppi(), piece.getRotation(), piece.getLocation(), Julkinen::Koordinaatti());
		// Check that there is an item on piece
		if (piece.getItem() != 'x')
			mScreen->esineLaudalle(piece.getItem(), piece.getLocation());
	});
	std::for_each(mPlayers.begin(), mPlayers.end(), [&](Player player){
		mScreen->pelaajaLaudalle(player.getAbbr(), player.getLocation());
	});
	std::for_each(mPlayers.begin(), mPlayers.end(), [&](Player player){
		mScreen->tulostaPelaajantiedot(player.getName(), player.getCollectedItems(), player.getTargetItems(), player.getLastCommand());
	});
}
bool Game::isCollision(const Julkinen::Suunta& direction, const unsigned& amount){

	// If direction is PAIKALLAAN or AUTOMAATTI, no reason to continue in method
	if (direction == Julkinen::PAIKALLAAN || direction == Julkinen::AUTOMAATTI)
		return false;

	// Get iterator to player
	std::vector<Player>::iterator player = mPlayers.begin() + mActivePlayer;

	// Get player location
	int x = player->getLocation().haeXkoordinaatti();
	int y = player->getLocation().haeYkoordinaatti();

	switch (direction){
	case Julkinen::ALAS:
		// check if out of game area
		if ((y + (int)amount) > mAreaSize) return true;
		// check for collision
		for (int i = 0; i < (int)amount; i++){

			std::vector<Piece>::iterator currentPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
				return piece.getLocation() == Julkinen::Koordinaatti(x, y);
			});

			Julkinen::PalaTyyppi startType = currentPiece->getType();
			unsigned int startRotation = currentPiece->getRotation();

			if ((startType == Julkinen::IPALA && startRotation == 2)
				|| (startType == Julkinen::IPALA && startRotation == 4)
				|| (startType == Julkinen::LPALA && startRotation == 1)
				|| (startType == Julkinen::LPALA && startRotation == 4)
				|| (startType == Julkinen::TPALA && startRotation == 3)
				) return true;

			if (y < mAreaSize){
				std::vector<Piece>::iterator nextPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
					return piece.getLocation() == Julkinen::Koordinaatti(x, y + 1);
				});

				Julkinen::PalaTyyppi destType = nextPiece->getType();
				unsigned int destRotation = nextPiece->getRotation();

				if ((destType == Julkinen::IPALA && destRotation == 2)
					|| (destType == Julkinen::IPALA && destRotation == 4)
					|| (destType == Julkinen::LPALA && destRotation == 2)
					|| (destType == Julkinen::LPALA && destRotation == 3)
					|| (destType == Julkinen::TPALA && destRotation == 1)
					) return true;
			}
			y++;
		}
		if (playerToPlayerCollision(x, y))
			return true;
		break;
	case Julkinen::YLOS:
		// check if out of game area
		if ((y - (int)amount) < 1) return true;
		// check for collision
		for (unsigned i = 0; i < amount; i++){

			std::vector<Piece>::iterator currentPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
				return piece.getLocation() == Julkinen::Koordinaatti(x, y);
			});

			Julkinen::PalaTyyppi startType = currentPiece->getType();
			unsigned int startRotation = currentPiece->getRotation();

			if ((startType == Julkinen::IPALA && startRotation == 2)
				|| (startType == Julkinen::IPALA && startRotation == 4)
				|| (startType == Julkinen::LPALA && startRotation == 3)
				|| (startType == Julkinen::LPALA && startRotation == 3)
				|| (startType == Julkinen::TPALA && startRotation == 1)
				) return true;

			if (y > 1){
				std::vector<Piece>::iterator nextPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
					return piece.getLocation() == Julkinen::Koordinaatti(x, y - 1);
				});

				Julkinen::PalaTyyppi destType = nextPiece->getType();
				unsigned int destRotation = nextPiece->getRotation();

				if ((destType == Julkinen::IPALA && destRotation == 2)
					|| (destType == Julkinen::IPALA && destRotation == 4)
					|| (destType == Julkinen::LPALA && destRotation == 1)
					|| (destType == Julkinen::LPALA && destRotation == 4)
					|| (destType == Julkinen::TPALA && destRotation == 3)
					) return true;
			}
			y--;

		}
		if (playerToPlayerCollision(x, y))
			return true;
		break;
	case Julkinen::OIKEALLE:
		// check if out of game area
		if ((x + (int)amount) > mAreaSize) return true;
		// check for collision
		for (int i = 0; i < (int)amount; i++){

			std::vector<Piece>::iterator currentPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
				return piece.getLocation() == Julkinen::Koordinaatti(x, y);
			});

			Julkinen::PalaTyyppi startType = currentPiece->getType();
			unsigned int startRotation = currentPiece->getRotation();

			if ((startType == Julkinen::IPALA && startRotation == 1)
				|| (startType == Julkinen::IPALA && startRotation == 3)
				|| (startType == Julkinen::LPALA && startRotation == 3)
				|| (startType == Julkinen::LPALA && startRotation == 4)
				|| (startType == Julkinen::TPALA && startRotation == 2)
				) return true;

			if (x < mAreaSize){
				std::vector<Piece>::iterator nextPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
					return piece.getLocation() == Julkinen::Koordinaatti(x + 1, y);
				});

				Julkinen::PalaTyyppi destType = nextPiece->getType();
				unsigned int destRotation = nextPiece->getRotation();

				if ((destType == Julkinen::IPALA && destRotation == 1)
					|| (destType == Julkinen::IPALA && destRotation == 3)
					|| (destType == Julkinen::LPALA && destRotation == 1)
					|| (destType == Julkinen::LPALA && destRotation == 2)
					|| (destType == Julkinen::TPALA && destRotation == 4)
					) return true;
			}
			x++;
		}
		// Check for player collision
		if (playerToPlayerCollision(x, y))
			return true;
		break;
	case Julkinen::VASEMMALLE:
		// check if out of game area
		if ((x - (int)amount) < 1)
			return true;
		// check for collision
		for (int i = 0; i < (int)amount; i++){

			std::vector<Piece>::iterator currentPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
				return piece.getLocation() == Julkinen::Koordinaatti(x, y);
			});

			Julkinen::PalaTyyppi startType = currentPiece->getType();
			unsigned int startRotation = currentPiece->getRotation();

			if ((startType == Julkinen::IPALA && startRotation == 1)
				|| (startType == Julkinen::IPALA && startRotation == 3)
				|| (startType == Julkinen::LPALA && startRotation == 1)
				|| (startType == Julkinen::LPALA && startRotation == 2)
				|| (startType == Julkinen::TPALA && startRotation == 4)
				) return true;

			if (x > 1){
				std::vector<Piece>::iterator nextPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
					return piece.getLocation() == Julkinen::Koordinaatti(x - 1, y);
				});

				Julkinen::PalaTyyppi destType = nextPiece->getType();
				unsigned int destRotation = nextPiece->getRotation();

				if ((destType == Julkinen::IPALA && destRotation == 1)
					|| (destType == Julkinen::IPALA && destRotation == 3)
					|| (destType == Julkinen::LPALA && destRotation == 3)
					|| (destType == Julkinen::LPALA && destRotation == 4)
					|| (destType == Julkinen::TPALA && destRotation == 2)
					) return true;
			}
			x--;
		}
		if (playerToPlayerCollision(x, y))
			return true;
		break;
	}
	// If got to this point, no collisions detected
	return false;
}
bool Game::playerToPlayerCollision(const int& x, const int& y){
	for (int i = 0; i < (int)mPlayers.size(); i++){
		if (mPlayers.at(i).getLocation() == Julkinen::Koordinaatti(x, y) && mPlayers.at(i).getName() != mPlayers.at(mActivePlayer).getName()){
			return true;
		}
	};
	return false;
}
void Game::handleCPU(){

	// Get CPU target coordinates
	Julkinen::Koordinaatti targetCoords;
	std::for_each(mPieces.begin(), mPieces.end(), [&](Piece piece){
		if (piece.getItem() == mPlayers.at(mActivePlayer).nextItem())
			targetCoords = piece.getLocation();
	});

	// If current CPU target is on a detached piece, do nothing and return
	if (targetCoords.onkoIrtopala())
		return;

	DEBUG_OUTPUT("CPU Target coords: " << targetCoords.haeXkoordinaatti() << " " << targetCoords.haeYkoordinaatti() << std::endl);

	// Move to closest distance
	Julkinen::Koordinaatti playerCoords = mPlayers.at(mActivePlayer).getLocation();
	int diffX, diffY, moveDistance;
	Julkinen::Suunta priorityDirection;
	diffX = playerCoords.haeXkoordinaatti() - targetCoords.haeXkoordinaatti();
	diffY = playerCoords.haeYkoordinaatti() - targetCoords.haeYkoordinaatti();
	// Calculate priority direction 
	if (std::abs(diffX) > std::abs(diffY)){
		if (diffX > 0)
		{
			priorityDirection = Julkinen::VASEMMALLE;
			moveDistance = std::abs(diffX);
		}
		else if (diffX < 0)
		{
			priorityDirection = Julkinen::OIKEALLE;
			moveDistance = std::abs(diffX);
		}
	}
	else if (std::abs(diffY) > std::abs(diffX)){
		if (diffY > 0)
		{
			priorityDirection = Julkinen::YLOS;
			moveDistance = std::abs(diffY);
		}
		else if (diffY < 0)
		{
			priorityDirection = Julkinen::ALAS;
			moveDistance = std::abs(diffY);
		}
	}
	// if there is no difference between diffY and diffX, move to random direction
	else{
		moveDistance = 1;
		priorityDirection = randomDirection();
	}

	// Test priority direction for collisions.
	// If test fails, reduce moveDistance by one and try again.
	bool safeToMove = false;
	while (moveDistance > 0){
		if (isCollision(priorityDirection, moveDistance))
			moveDistance--;
		else{
			safeToMove = true;
			break;
		}

	}
	// If could not move, try move to random direction as far as possible
	if (!safeToMove){
		Julkinen::Suunta rndDirection = priorityDirection;

		// Find a direction other than current priority
		while (rndDirection == priorityDirection)
			rndDirection = randomDirection();

		// Set it as new priority direction
		priorityDirection = rndDirection;
		moveDistance = 1;

		// Check for collisions
		while (moveDistance > 0){
			if (isCollision(priorityDirection, moveDistance))
				moveDistance--;
			else{
				safeToMove = true;
				break;
			}

		}
	}
	// Confirm that it is safe and move CPU.
	// If its still not safe, do not move at all.
	if (safeToMove)
		movePlayer(moveDistance, priorityDirection);

	// Set CPU status flags
	mPlayerActionStatus.setMoved();
	mPlayerActionStatus.setPushed();
}
std::string Game::directionChar(const Julkinen::Suunta& direction){
	if (direction == Julkinen::ALAS)
		return std::string("a");
	else if (direction == Julkinen::YLOS)
		return std::string("y");
	else if (direction == Julkinen::VASEMMALLE)
		return std::string("v");
	else if (direction == Julkinen::OIKEALLE)
		return std::string("o");

	return std::string("");
}
void Game::movePlayer(const int& distance, const Julkinen::Suunta& direction){
	// Get iterator to player
	std::vector<Player>::iterator player = mPlayers.begin() + mActivePlayer;
	// Get players current coordinates
	int x = player->getLocation().haeXkoordinaatti();
	int y = player->getLocation().haeYkoordinaatti();
	switch (direction){
	case Julkinen::ALAS:
		// Loop all pieces in players path to collect items
		for (int limit = y + distance; y < limit + 1; y++){
			if (y <= mAreaSize){
				// Get iterator to piece under player
				std::vector<Piece>::iterator atPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
					return piece.getLocation() == Julkinen::Koordinaatti(x, y);
				});
				// If piece has item that matches players next target, collect it
				if (atPiece->getItem() == player->nextItem()){
					player->collectItem();
					atPiece->removeItem();
				}
				// If piece has special type of Teleport, do teleporting and leave method
				if (atPiece->getSpecialType() == Julkinen::TELEPORTTI){
					player->setLocation(atPiece->getTarget());
					mScreen->ilmoitusErikoispalaanAstuttu(Julkinen::TELEPORTTI, player->getName());
					return;
				}
			}
		}
		y--;
		// Move player
		player->setLocation(Julkinen::Koordinaatti(x, y));
		break;
	case Julkinen::YLOS:
		// Loop all pieces in players path to collect items
		for (int limit = y - distance; y >= limit; y--){
			if (y > 0){
				// Get iterator to piece under player
				std::vector<Piece>::iterator atPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
					return piece.getLocation() == Julkinen::Koordinaatti(x, y);
				});
				// If piece has item that matches players next target, collect it
				if (atPiece->getItem() == player->nextItem()){
					player->collectItem();
					mScreen->ilmoitusEsinePoimittu(atPiece->getItem(), player->getName());
					atPiece->removeItem();
				}
				// If piece has special type of Teleport, do teleporting and leave method
				if (atPiece->getSpecialType() == Julkinen::TELEPORTTI){
					player->setLocation(atPiece->getTarget());
					mScreen->ilmoitusErikoispalaanAstuttu(Julkinen::TELEPORTTI, player->getName());
					return;
				}
			}
		}
		y++;
		// Move player
		player->setLocation(Julkinen::Koordinaatti(x, y));
		break;
	case Julkinen::OIKEALLE:
		// Loop all pieces in players path to collect items
		for (int limit = x + distance; x < limit + 1; x++){
			if (x <= mAreaSize){
				// Get iterator to piece under player
				std::vector<Piece>::iterator atPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
					return piece.getLocation() == Julkinen::Koordinaatti(x, y);
				});
				// If piece has item that matches players next target, collect it
				if (atPiece->getItem() == player->nextItem()){
					player->collectItem();
					atPiece->removeItem();
				}
				// If piece has special type of Teleport, do teleporting and leave method
				if (atPiece->getSpecialType() == Julkinen::TELEPORTTI){
					player->setLocation(atPiece->getTarget());
					mScreen->ilmoitusErikoispalaanAstuttu(Julkinen::TELEPORTTI, player->getName());
					return;
				}
			}
		}
		x--;
		// Move player
		player->setLocation(Julkinen::Koordinaatti(x, y));
		break;
	case Julkinen::VASEMMALLE:
		// Loop all pieces in players path to collect items
		for (int limit = x - distance; x >= limit; x--){
			if (x > 0){
				// Get iterator to piece under player
				std::vector<Piece>::iterator atPiece = std::find_if(mPieces.begin(), mPieces.end(), [&](Piece piece){
					return piece.getLocation() == Julkinen::Koordinaatti(x, y);
				});
				// If piece has item that matches players next target, collect it
				if (atPiece->getItem() == player->nextItem()){
					player->collectItem();
					atPiece->removeItem();
				}
				// If piece has special type of Teleport, do teleporting and leave method
				if (atPiece->getSpecialType() == Julkinen::TELEPORTTI){
					player->setLocation(atPiece->getTarget());
					mScreen->ilmoitusErikoispalaanAstuttu(Julkinen::TELEPORTTI, player->getName());
					return;
				}
			}
		}
		x++;
		// Move player
		player->setLocation(Julkinen::Koordinaatti(x, y));
		break;
	}
}
Julkinen::Suunta Game::randomDirection(){
	srand((unsigned)time(NULL));
	int rnd = rand() % 4;
	switch (rnd){
	case 0:
		return Julkinen::YLOS;
	case 1:
		return Julkinen::ALAS;
	case 2:
		return Julkinen::VASEMMALLE;
	case 3:
		return Julkinen::OIKEALLE;
	default:
		return Julkinen::Suunta::PAIKALLAAN;
	}

}
bool Game::freePieceSlot(const Julkinen::Koordinaatti& coord){
	std::vector<Piece>::iterator piece = mPieces.end();
	std::find_if(mPieces.begin(), mPieces.end(), [&coord](Piece piece){
		return piece.getLocation() == coord;
	});
	if (piece == mPieces.end())
		return true;

	return false;
}