#pragma once
#include <memory>
#include <map>
#include <algorithm>
#include <climits>
#include <iterator>
#include <cmath>
#include <ctime>
#include "pelirajapinta.hh"
#include "koordinaatti.hh"
#include "toimintovirhe.hh"
#include "komentovirhe.hh"
#include "valmiiden_toteutus\include\naytto.hh"

#include "Player.h"
#include "Piece.h"

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
	bool mInitialization;
	Naytto* mScreen;
	unsigned mAreaSize;
	std::vector<Player> mPlayers;
	std::vector<Piece> mPieces;
	int mActivePlayer;
	bool mHasPushed;

	// New methods
	void updateScreen();
	bool isCollision(Julkinen::Suunta direction, unsigned int amount);
	bool playerToPlayerCollision(int x, int y);
	void handleCPU();
	std::string directionChar(Julkinen::Suunta direction);
	void movePlayer(int distance, Julkinen::Suunta direction);
	Julkinen::Suunta randomDirection();
};

