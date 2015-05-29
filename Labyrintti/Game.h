#pragma once
#include <memory>
#include <map>
#include <algorithm>
#include <climits>
#include <iterator>
#include <cmath>
#include <ctime>
#include <exception>
#include "pelirajapinta.hh"
#include "koordinaatti.hh"
#include "toimintovirhe.hh"
#include "komentovirhe.hh"
#include "alustusvirhe.hh"
#include "valmiiden_toteutus\include\naytto.hh"

#include "Player.h"
#include "Piece.h"
#include "PlayerActionStatus.h"

class Game :
	public Julkinen::Pelirajapinta
{
public:
	Game();
	~Game();

	// Inherited methods
	bool onkoAlustustilassa() const;
	void lisaaNaytto(Julkinen::Nayttorajapinta* naytto);
	void maaritaPelialueenKoko(Julkinen::Koordinaatti const& koko);
	void lisaaPelaaja(Julkinen::PelaajaTyyppi tyyppi, std::string const& nimi, char lyhenne, Julkinen::Koordinaatti const& sijainti);
	void lisaaPala(Julkinen::PalaTyyppi pala, unsigned int rotaatio, Julkinen::Koordinaatti const& sijainti);
	void lisaaEsine(char merkki, Julkinen::Koordinaatti const& sijainti, std::string const& pelaaja);
	void asetaPalanTyyppi(Julkinen::ErikoispalaTyyppi tyyppi, Julkinen::Koordinaatti const& sijainti, Julkinen::Koordinaatti const& kohde = Julkinen::Koordinaatti());
	void alustusLopeta();
	bool onkoPelitilassa() const;
	void komentoTyonna(Julkinen::Reuna reuna, unsigned int paikka, unsigned int rotaatio);
	void komentoLiiku(Julkinen::Suunta suunta, unsigned int maara = 0);
	bool vaihdaVuoro();
	Julkinen::PelaajaTyyppi haeVuorossa();



private:
	bool mGameMode;
	Naytto* mScreen;
	int mAreaSize;
	std::vector<Player> mPlayers;
	std::vector<Piece> mPieces;
	int mActivePlayer;
	PlayerActionStatus mPlayerActionStatus;


	// New methods
	void updateScreen();
	bool isCollision(const Julkinen::Suunta& direction, const unsigned& amount);
	bool playerToPlayerCollision(const int& x, const int& y);
	void handleCPU();
	std::string directionChar(const Julkinen::Suunta& direction);
	void movePlayer(const int& distance, const Julkinen::Suunta& direction);
	Julkinen::Suunta randomDirection();
	bool freePieceSlot(const Julkinen::Koordinaatti& coord);
};