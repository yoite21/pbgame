#include <time.h>

#include "pbGameClient.h"

void pbGameClient::render()
{
	world.render();
}

pbGameClient::pbGameClient()
{
	moveKeyFlag = 0;
}

int pbGameClient::init()
{
	glutSetCursor(GLUT_CURSOR_NONE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	
	SetCursorPos(100,100);
	srand(time(NULL));

	
	network.setNetworkType();
	world.loadMap();	

	if ( network.getNetworkType() == SINGLE)
	{
		world.genInitialGhostPosition();
		world.genInitialPlayerPosition();
		world.addHostPlayer();
		world.addSinglePlayer();
	}
	else if( network.getNetworkType() == HOST)
	{


		world.genInitialGhostPosition();
		world.genInitialPlayerPosition();
		world.addHostPlayer();

		
		network.initNetwork();
		network.initHost();
		
		// initial setting position to send buffer
		network.setSendBuffer(world.getPositionInfo(true));
		
		network.createThread();

	}
	else
	{
		network.initNetwork();
		network.connectToHost();
		
		world.setMyPlayerId(network.getMyPlayerId());

		// initial setting position from recv buffer
		world.updatePlayersPosition(network.getRecvBufferVector());

		network.createThread();
	}

	
	world.loadModel();
	

	return 0;

}

void pbGameClient::updateNetwork()
{
	if ( network.getNetworkType() == SINGLE ) 
		return;
	else if ( network.getNetworkType() == HOST )
	{
		int newPlayerId = network.getNewPlayerId();
		int disconnectPlayerId = network.getDisconnectPlayerId();
		
		world.addClientPlayer(newPlayerId);
		world.disconnectClientPlayer(disconnectPlayerId);
		
		world.updatePlayersPosition(network.getRecvBufferVector());
		network.setSendBuffer(world.getPositionInfo((network.getNetworkType() == HOST) ? true: false));

		network.sendInitInfo(newPlayerId);
	}
	else
	{
		world.updatePlayersPosition(network.getRecvBufferVector());
		network.setSendBuffer(world.getPositionInfo((network.getNetworkType() == HOST) ? true: false));
	}
}


void pbGameClient::move()
{
	world.advanceModels();

	if(moveKeyFlag)
	{
		int frontDirection = 0, rightDirection = 0;
		if(w_key)
			frontDirection += 1;
		if(a_key)
			rightDirection -= 1;
		if(s_key)
			frontDirection -= 1;
		if(d_key)
			rightDirection += 1;


		world.playerMove(frontDirection, rightDirection);
	}

	if( network.getNetworkType() == HOST || network.getNetworkType() == SINGLE)
	{
		world.ghostMove();
	}
}


void pbGameClient::normalKeyboardPress(unsigned char key)
{
	

	moveKeyFlag++;

	if(key == 'w')
		w_key = true;
	else if(key == 'a')
		a_key = true;
	else if(key == 's')
		s_key = true;
	else if(key == 'd')
		d_key = true;
	else if(key == ' ')
		world.tryCatchPlayer();
	else if(key == 27)
		exit(0);
	else
		moveKeyFlag--;
}

void pbGameClient::normalKeyboardUp(unsigned char key)
{
	moveKeyFlag--;
	if(key == 'w')
		w_key = false;
	else if(key == 'a')
		a_key = false;
	else if(key == 's')
		s_key = false;
	else if(key == 'd')
		d_key = false;
	else if(key == ' ')
		space_key = false;
	else
		moveKeyFlag++;
}

void pbGameClient::mouseMove(float x, float y)
{
	world.cameraMove(x, y);
}