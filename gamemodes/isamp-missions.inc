#if defined _isamp_missions_included
	#endinput
#endif
#define _isamp_missions_included

//===================SISTEMA DE MINIMISIONES AUTOMATICAS========================

//=================================CONSTANTES===================================

#define MISSION_EVENT_TIME      30 // En minutos, tiempo m�nimo entre misiones.
#define MISSION_EVENT_EXTRATIME 30 // En minutos, m�ximo que se le puede agregar al tiempo m�nimo entre misiones.

#define MISSION_NONE            0
#define MISSION_MAT             1
#define MISSION_DRUG            2
#define MISSION_CAR             3

#define NUM_MISSION          	424

//=================================VARIABLES====================================

new pMissionEvent[MAX_PLAYERS],
	pMissionEventTimer[MAX_PLAYERS],
	pMissionEventStep[MAX_PLAYERS],
	pMissionEventParam[MAX_PLAYERS],
	pMissionEventParam2[MAX_PLAYERS];

//============================FUNCIONES PUBLICAS================================

forward CreateMissionEventTimer(playerid); // Crea el timer inicial que le dar� misiones a playerid mientras est� conectado.
forward KillMissionEventTimer(playerid); // Destruye el timer que genera misiones a playerid.
forward bool:CheckMissionEventStep1(playerid); // Devuelve true si se efectu� exitosamente el paso de la etapa 1 a la 2 (o final) de la mision activa, false en caso contrario.
forward bool:CheckCancelMissionEventStep1(playerid); // Devuelve true si se efectu� exitosamente la cancelaci�n de la mision activa en la etapa 1.
forward bool:CheckMissionEventStep2(playerid, text[]); // Devuelve true si se efectu� exitosamente el paso de la etapa 2 a la 3 (o final) de la mision activa, false en caso contrario.
forward bool:CheckMissionEventStep3(playerid, text[]); // Devuelve true si se efectu� exitosamente el paso de la etapa 3 a la 4 (o final) de la mision activa, false en caso contrario.
forward CheckMissionEventStep4(playerid);
forward ResetMissionEventVariables(playerid); // Resetea a valores default las variables del sistema.

//============================FUNCIONES INTERNAS================================

forward MissionEvent(playerid); // Public para crear las misiones. Utilizado por el timer principal.
forward CancelMissionEvent(playerid); // Cancela la mision si no respondi� en cierto tiempo.

forward MissionMatLoad(playerid, vehicleid); // Public correspondiente a la carga de materiales de la mision MISSION_MAT.
forward MissionMatUnload(playerid, matsCount, vehicleid); // Public correspondiente a la descarga de materiales de la mision MISSION_MAT que termina la misi�n.

//=========================IMPLEMENTACI�N DE FUNCIONES==========================

stock CreateMissionEventTimer(playerid)
{
	if(PlayerInfo[playerid][pFaction] > 0 && FactionInfo[PlayerInfo[playerid][pFaction]][fType] == FAC_TYPE_ILLEGAL)
	    pMissionEventTimer[playerid] = SetTimerEx("MissionEvent", 60 * 1000 * (MISSION_EVENT_TIME + random(MISSION_EVENT_EXTRATIME)), false, "i", playerid); // No es recursivo para hacer uso del random en tiempo.
}

stock KillMissionEventTimer(playerid)
{
	KillTimer(pMissionEventTimer[playerid]);
}

stock ResetMissionEventVariables(playerid)
{
	pMissionEvent[playerid] = MISSION_NONE;
	pMissionEventStep[playerid] = 0;
	pMissionEventParam[playerid] = 0;
	pMissionEventParam2[playerid] = 0;
}

public MissionEvent(playerid)
{
	new factionID = PlayerInfo[playerid][pFaction];

	pMissionEventTimer[playerid] = SetTimerEx("MissionEvent", 60 * 1000 * (MISSION_EVENT_TIME + random(MISSION_EVENT_EXTRATIME)), false, "i", playerid);

	if(PlayerInfo[playerid][pJailed] != 0 || random(2) == 0 || pMissionEvent[playerid] != MISSION_NONE)
	    return 1;

	if(factionID > 0 && FactionInfo[factionID][fType] == FAC_TYPE_ILLEGAL)
	{
		PlayerDoMessage(playerid, 15.0, "Un tel�fono ha comenzado a sonar.");
		SendClientMessage(playerid, COLOR_WHITE, "Tienes una llamada, utiliza /atender o /colgar.");
		pMissionEvent[playerid] = 1 + random(3); // Tipo de misi�n aleatorio.
		SetTimerEx("CancelMissionEvent", 35000, false, "i", playerid);
		pMissionEventStep[playerid] = 1;
	}
	return 1;
}

public CancelMissionEvent(playerid)
{
	if(pMissionEventStep[playerid] == 1)
 	{
		PlayerDoMessage(playerid, 15.0, "Han colgado...");
        ResetMissionEventVariables(playerid);
	}
	return 1;
}

stock bool:CheckMissionEventStep1(playerid)
{
    if(pMissionEventStep[playerid] == 1)
	{
	    switch(pMissionEvent[playerid])
		{
		    case MISSION_MAT:
		 		SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: nos ha llegado un cargamento de materiales, �vienes a buscarlo?. Tambi�n recibir�s efectivo.");
		    case MISSION_DRUG:
		 		SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: ando necesitando algo de merca, �tienes algo disponible?. Pago bien.");
		    case MISSION_CAR:
		    	SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: tengo un trabajo para ti: cons�gueme el auto que te indique. Dinero facil, �qu� dices?");
		}
  		Mobile[playerid] = NUM_MISSION;
		pMissionEventStep[playerid] = 2;
		return true;
	}
	return false;
}

stock bool:CheckCancelMissionEventStep1(playerid)
{
	if(pMissionEventStep[playerid] == 1)
	{
	    PlayerActionMessage(playerid, 15.0, "cuelga y guarda su tel�fono celular en el bolsillo.");
     	ResetMissionEventVariables(playerid);
	    return true;
	}
	return false;
}

stock bool:CheckMissionEventStep2(playerid, text[])
{
	if(Mobile[playerid] == NUM_MISSION && pMissionEventStep[playerid] == 2)
	{
		if((strcmp("si", text, true) == 0) && (strlen(text) == strlen("si")))
		{
		    switch(pMissionEvent[playerid])
			{
			    case MISSION_MAT:
			    {
					SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: bien, s�bete a la van y di 'listo' una vez arriba.");
		            pMissionEventStep[playerid] = 3;
		            jobDuty[playerid] = true;
		            SetVehicleParamsForPlayer(FactionInfo[PlayerInfo[playerid][pFaction]][fMissionVeh], playerid, 1, 0);
				}
				case MISSION_DRUG:
				{
				    pMissionEventParam[playerid] = 30 + random(20);
				    switch(random(3))
				    {
				        case 0:
				        {
				            pMissionEventParam2[playerid] = 0;
				            SendFMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: Escucha, busco %d gramos de marihuana. �Tienes esa cantidad?", pMissionEventParam[playerid]);
						}
				        case 1:
				        {
				            pMissionEventParam2[playerid] = 1;
				            SendFMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: Escucha, busco %d dosis de lsd. �Tienes esa cantidad?", pMissionEventParam[playerid]);
						}
				        case 2:
				        {
				            pMissionEventParam2[playerid] = 2;
				            SendFMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: Escucha, busco %d pastillas de �xtasis. �Tienes esa cantidad?", pMissionEventParam[playerid]);
						}
				        case 3:
				        {
				            pMissionEventParam2[playerid] = 3;
				            SendFMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: Escucha, busco %d gramos de coca�na. �Tienes esa cantidad?", pMissionEventParam[playerid]);
						}
					}
     				pMissionEventStep[playerid] = 3;
			 	}
			    case MISSION_CAR:
			    {
			        new bool:isCar = false,
			            vehicleid;
			            
			        while(isCar == false)
			        {
			   			vehicleid = 1 + random(MAX_VEH - 1);
			        	if(GetVehicleType(vehicleid) == VTYPE_CAR)
			        	    isCar = true;
					}
					pMissionEventParam[playerid] = vehicleid;
					pMissionEventStep[playerid] = 3;
					SendFMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: Escucha, necesito que me consigas un %s en buen estado. �Puedes hacerlo?", GetVehicleName(vehicleid));
				}
			}
		}
		else if((strcmp("no", text, true) == 0) && (strlen(text) == strlen("no")))
		{
			SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: t� te lo pierdes, adi�s...");
            PlayerActionMessage(playerid, 15.0, "cuelga y guarda su tel�fono celular en el bolsillo.");
            Mobile[playerid] = 255;
            ResetMissionEventVariables(playerid);
		}
		else
			SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: �No tengo tiempo para vueltas, di 'si' o 'no'!");
		return true;
	}
	return false;
}

stock bool:CheckMissionEventStep3(playerid, text[])
{
	if(Mobile[playerid] == NUM_MISSION && pMissionEventStep[playerid] == 3)
	{
	    switch(pMissionEvent[playerid])
	    {
	        case MISSION_MAT:
	        {
				if((strcmp("listo", text, true) == 0) && (strlen(text) == strlen("listo")))
				{
				    if(GetPlayerVehicleID(playerid) == FactionInfo[PlayerInfo[playerid][pFaction]][fMissionVeh])
					{
						SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: te he marcado la localizaci�n en el GPS, v� a buscar los paquetes y ll�valos a tu HQ.");
			            SetPlayerCheckpoint(playerid, 2792.4609, -2417.5508, 13.7599, 4.0);
			            pMissionEventStep[playerid] = 4;
			            Mobile[playerid] = 255;
						PlayerActionMessage(playerid, 15.0, "guarda su tel�fono celular en el bolsillo.");
					} else
					    SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: �s�bete a la van, no tengo tiempo para vueltas!");
				} else
					SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: �no tengo tiempo para vueltas, di 'listo' una vez arriba!");
			}
			case MISSION_DRUG:
			{
				if((strcmp("si", text, true) == 0) && (strlen(text) == strlen("si")))
				{
					SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: Excelente, llevame la mercancia al lugar que te he pasado al celular. No tardes.");
                    SetPlayerCheckpoint(playerid, 2465.5369, -955.8604, 80.0118, 4.0);
					pMissionEventStep[playerid] = 4;
			        Mobile[playerid] = 255;
           			jobDuty[playerid] = true;
					PlayerActionMessage(playerid, 15.0, "guarda su tel�fono celular en el bolsillo.");
				}
				else if((strcmp("no", text, true) == 0) && (strlen(text) == strlen("no")))
				{
					SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: t� te lo pierdes, adi�s...");
		            PlayerActionMessage(playerid, 15.0, "cuelga y guarda su tel�fono celular en el bolsillo.");
		            Mobile[playerid] = 255;
              		ResetMissionEventVariables(playerid);
				}
				else
					SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: �No tengo tiempo para vueltas, di 'si' o 'no'!");
			}
			case MISSION_CAR:
			{
				if((strcmp("si", text, true) == 0) && (strlen(text) == strlen("si")))
				{
					SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: Perfecto, llevame rapidamente el veh�culo al lugar que te he pasado por el celular.");
                    SetPlayerCheckpoint(playerid, 91.0594, -164.6225, 2.5036, 4.0);
					pMissionEventStep[playerid] = 4;
			        Mobile[playerid] = 255;
           			jobDuty[playerid] = true;
					PlayerActionMessage(playerid, 15.0, "guarda su tel�fono celular en el bolsillo.");
				}
				else if((strcmp("no", text, true) == 0) && (strlen(text) == strlen("no")))
				{
					SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: t� te lo pierdes, adi�s...");
		            PlayerActionMessage(playerid, 15.0, "cuelga y guarda su tel�fono celular en el bolsillo.");
		            Mobile[playerid] = 255;
              		ResetMissionEventVariables(playerid);
				}
				else
					SendClientMessage(playerid, COLOR_FADE1, "[Voz al tel�fono]: �No tengo tiempo para vueltas, di 'si' o 'no'!");
			}
		}
		return true;
	}
	return false;
}

stock CheckMissionEventStep4(playerid)
{
	switch(pMissionEvent[playerid])
	{
	    case MISSION_MAT:
	    {
	        new vehicleid = GetPlayerVehicleID(playerid),
	            factionid = PlayerInfo[playerid][pFaction];
	        
			if(pMissionEventStep[playerid] == 5)
			{
			    new matsCount = 0;

			    for(new i = 0; i < GetVehicleMaxTrunkSlots(vehicleid); i++)
				{
				    if(GetTrunkItem(vehicleid, i) == ITEM_ID_MATERIALES)
					{
				        matsCount = GetTrunkParam(vehicleid, i);
						FactionInfo[factionid][fMaterials] += matsCount;
						SetTrunkItemAndParam(vehicleid, i, 0, 0);
				    }
				}
				GameTextForPlayer(playerid, "Descargando materiales del veh�culo...", 4000, 4);
				TogglePlayerControllable(playerid, false);
				SetTimerEx("MissionMatUnload", 4000, false, "ii", playerid, matsCount);
			}
			else if(pMissionEventStep[playerid] == 4)
			{
			    if(FactionInfo[factionid][fMissionVeh] == vehicleid)
				{
			        GameTextForPlayer(playerid, "Cargando materiales...", 4000, 4);
					TogglePlayerControllable(playerid, false);
					SetTimerEx("MissionMatLoad", 4000, false, "i", playerid, vehicleid);
				} else {
				    SetPlayerCheckpoint(playerid, 2792.4609, -2417.5508, 13.7599, 5.4);
				    GameTextForPlayer(playerid, "Debes estar en el veh�culo de la misi�n.", 1000, 4);
				}
			}
		}
		case MISSION_DRUG:
		{
			if(pMissionEventStep[playerid] == 4)
			{
			    switch(pMissionEventParam2[playerid])
			    {
			        case 0:
			        {
			            if(PlayerInfo[playerid][pMarijuana] >= pMissionEventParam[playerid])
			            {
			                SendFMessage(playerid, COLOR_WHITE, "Sujeto desconocido: Bien, veo que me has traido lo que te ped�. Aqui tienes $%d por la mercanc�a.", pMissionEventParam[playerid] * 50);
							GivePlayerCash(playerid, pMissionEventParam[playerid] * 50);
							PlayerInfo[playerid][pMarijuana] -= pMissionEventParam[playerid];
							PlayerActionMessage(playerid, 8.0, "Le entrega un paquete a un sujeto desconocido y �ste se aleja rapidamente.");
						} else
			        	    SendClientMessage(playerid, COLOR_WHITE, "Sujeto desconocido: No tienes la cantidad que te he pedido idiota. Me haces perder el tiempo.");
			        }
			        case 1:
			        {
			            if(PlayerInfo[playerid][pLSD] >= pMissionEventParam[playerid])
			            {
			                SendFMessage(playerid, COLOR_WHITE, "Sujeto desconocido: Bien, veo que me has traido lo que te ped�. Aqui tienes $%d por la mercanc�a.", pMissionEventParam[playerid] * 60);
							GivePlayerCash(playerid, pMissionEventParam[playerid] * 60);
							PlayerInfo[playerid][pLSD] -= pMissionEventParam[playerid];
							PlayerActionMessage(playerid, 8.0, "Le entrega un paquete a un sujeto desconocido y �ste se aleja rapidamente.");
						} else
			        	    SendClientMessage(playerid, COLOR_WHITE, "Sujeto desconocido: No tienes la cantidad que te he pedido idiota. Me haces perder el tiempo.");
			        }
			        case 2:
			        {
			            if(PlayerInfo[playerid][pEcstasy] >= pMissionEventParam[playerid])
			            {
			                SendFMessage(playerid, COLOR_WHITE, "Sujeto desconocido: Bien, veo que me has traido lo que te ped�. Aqui tienes $%d por la mercanc�a.", pMissionEventParam[playerid] * 70);
							GivePlayerCash(playerid, pMissionEventParam[playerid] * 70);
							PlayerInfo[playerid][pEcstasy] -= pMissionEventParam[playerid];
							PlayerActionMessage(playerid, 8.0, "Le entrega un paquete a un sujeto desconocido y �ste se aleja rapidamente.");
						} else
			        	    SendClientMessage(playerid, COLOR_WHITE, "Sujeto desconocido: No tienes la cantidad que te he pedido idiota. Me haces perder el tiempo.");
			        }
			        case 3:
			        {
			            if(PlayerInfo[playerid][pCocaine] >= pMissionEventParam[playerid])
			            {
			                SendFMessage(playerid, COLOR_WHITE, "Sujeto desconocido: Bien, veo que me has traido lo que te ped�. Aqui tienes $%d por la mercanc�a.", pMissionEventParam[playerid] * 80);
							GivePlayerCash(playerid, pMissionEventParam[playerid] * 80);
							PlayerInfo[playerid][pCocaine] -= pMissionEventParam[playerid];
							PlayerActionMessage(playerid, 8.0, "Le entrega un paquete a un sujeto desconocido y �ste se aleja rapidamente.");
						} else
			        	    SendClientMessage(playerid, COLOR_WHITE, "Sujeto desconocido: No tienes la cantidad que te he pedido idiota. Me haces perder el tiempo.");
			        }
				}
        		ResetMissionEventVariables(playerid);
			}
		}
		case MISSION_CAR:
		{
  			if(pMissionEventStep[playerid] == 4)
			{
			    new vehicleid = GetPlayerVehicleID(playerid);
			    if(GetVehicleModel(vehicleid) == GetVehicleModel(pMissionEventParam[playerid]))
			    {
      				if(!playerHasCarKey(playerid, vehicleid))
      				{
      				    new Float:vhp;
      				    GetVehicleHealth(vehicleid, vhp);
      				    if(vhp > 500.0 && VehicleInfo[vehicleid][VehEngine] == 1)
      				    {
      				        SendFMessage(playerid, COLOR_WHITE, "Sujeto desconocido: Bien, en tiempo y forma. Aqui tienes $%d por el coche. Nunca nos vimos.", GetVehiclePrice(vehicleid, 100) / 180);
							GivePlayerCash(playerid, GetVehiclePrice(vehicleid, 100) / 180);
							SetVehicleToRespawn(vehicleid);
							PlayerActionMessage(playerid, 15.0, "Le entrega un veh�culo a un sujeto desconocido y recibe a cambio un paquete.");
					  	} else
      				        SendClientMessage(playerid, COLOR_WHITE, "Sujeto desconocido: Te dije que estuviese en buen estado y andando, no esta porquer�a. Dios mio...");
      				} else
	    				SendClientMessage(playerid, COLOR_YELLOW2, "No puedes entregar un auto del cual tienes las llaves. Se anula la misi�n.");
				} else
				    SendClientMessage(playerid, COLOR_WHITE, "Sujeto desconocido: Este no es el modelo que te hab�a pedido idiota, me haces perder el tiempo.");
        		ResetMissionEventVariables(playerid);
			}
		}
	}
}

//=================CARGA DE MATERIALES POR MISION PARA MAFIAS===================

public MissionMatLoad(playerid, vehicleid)
{
	new validslot = -1;

    TogglePlayerControllable(playerid, true);
    
	for(new i = 0; i < GetVehicleMaxTrunkSlots(vehicleid); i++)
	{
	    if(GetItemType(GetTrunkItem(vehicleid, i)) == ITEM_NONE)
		{
			validslot = i;
			break;
	    }
	}
	
	if(validslot == -1)
	{
        SendClientMessage(playerid, COLOR_YELLOW2, "El maletero se encuentra lleno, toma algo de �l y vuelve a intentarlo.");
		SetVehiclePos(vehicleid, 2767.2983, -2417.6804, 13.7573);
		SetPlayerCheckpoint(playerid, 2792.4609, -2417.5508, 13.7599, 5.4);
	} else
		{
		    foreach(new i : Player)
			{
			    if(playerid != i && PlayerInfo[i][pFaction] == PlayerInfo[playerid][pFaction] && GetPlayerVehicleID(i) == vehicleid)
					pMissionEventParam[playerid]++;
			}
			pMissionEventStep[playerid] = 5;
			SetTrunkItemAndParam(vehicleid, validslot, ITEM_ID_MATERIALES, MISSION_BOX_MATS + (MISSION_BOX_EXTRA * pMissionEventParam[playerid]) * (random(MISSION_MATS_MAX_BOXES) + 1 + MISSION_MATS_MIN_BOXES));
			SetPlayerCheckpoint(playerid, VehicleInfo[vehicleid][VehPosX], VehicleInfo[vehicleid][VehPosY], VehicleInfo[vehicleid][VehPosZ], 4.0);
			SendClientMessage(playerid, COLOR_LIGHTGREEN, "SMS de an�nimo: vuelve al HQ con la carga. Solo alguien con experiencia podr� ensamblarlas.");
		}
	return 1;
}

//================DESCARGA DE MATERIALES POR MISION PARA MAFIAS=================

public MissionMatUnload(playerid, matsCount, vehicleid)
{
	TogglePlayerControllable(playerid, true);
	SetVehicleParamsForPlayer(vehicleid, playerid, 0, 0);
	jobDuty[playerid] = false;
	SendFMessage(playerid, COLOR_WHITE, "Has descargado %d materiales en el HQ y recibido una ganancia de $%d.", matsCount, MISSION_MATS_REWARD + MISSION_MATS_EXTRA * pMissionEventParam[playerid]);
	GivePlayerCash(playerid, MISSION_MATS_REWARD + MISSION_MATS_EXTRA * pMissionEventParam[playerid]);
	SetVehicleToRespawn(vehicleid);
 	ResetMissionEventVariables(playerid);
    return 1;
}