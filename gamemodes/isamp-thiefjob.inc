#if defined _isamp_thiefjob_included
	#endinput
#endif
#define _isamp_thiefjob_included

enum tThiefJob {
	pFelonExp,
	pFelonLevel,
	pRobPersonLimit,
	pRobLastVictimPID,
	pTheftLastVictimPID,
	pTheftPersonLimit,
	pRob247Limit,
	pTheft247Limit,
	pRobHouseLimit,
	pRob2HouseLimit,
	pForceDoorLimit,
	pForceEngineLimit,
	pDesarmCarLimit
};
new ThiefJobInfo[MAX_PLAYERS][tThiefJob];

new policeCallTime[MAX_PLAYERS],
	theftTime[MAX_PLAYERS],
	felon[MAX_PLAYERS];
	
// Sistema de mascaras
new bool:usingMask[MAX_PLAYERS];

stock resetThiefVariables(playerid)
{
	ThiefJobInfo[playerid][pFelonExp] = 0;
	ThiefJobInfo[playerid][pFelonLevel] = 0;
	ThiefJobInfo[playerid][pRobPersonLimit] = 0;
	ThiefJobInfo[playerid][pRobLastVictimPID] = INVALID_PLAYER_ID;
	ThiefJobInfo[playerid][pTheftLastVictimPID] = INVALID_PLAYER_ID;
	ThiefJobInfo[playerid][pTheftPersonLimit] = 0;
	ThiefJobInfo[playerid][pRobHouseLimit] = 0;
	ThiefJobInfo[playerid][pRob2HouseLimit] = 0;
	ThiefJobInfo[playerid][pRob247Limit] = 0;
	ThiefJobInfo[playerid][pTheft247Limit] = 0;
	ThiefJobInfo[playerid][pForceDoorLimit] = 0;
	ThiefJobInfo[playerid][pForceEngineLimit] = 0;
	ThiefJobInfo[playerid][pDesarmCarLimit] = 0;

	theftTime[playerid] = -1;
	felon[playerid] = INVALID_PLAYER_ID;
	policeCallTime[playerid] = 0;
}

stock loadThiefJobData(playerid,MysqlID){
	new		
		query[256];

	#if defined _isamp_debug
		printf("\nLoading thiefJobData for player id: %d sqlid: %d\n",playerid,MysqlID);
	#endif
    format(query, sizeof(query), "SELECT * FROM `thief_job` WHERE accountid=%d", MysqlID);
	mysql_function_query(dbHandle, query, true, "OnThiefJobDataLoad", "i", playerid);
	return 1;
}

forward OnThiefJobDataLoad(playerid);
public OnThiefJobDataLoad(playerid) {
	new
		rows,
		fields,
		result[32];
	
	cache_get_data(rows, fields);
	if(rows)
	{	
		cache_get_field_content(0, "pFelonExp", result); 		ThiefJobInfo[playerid][pFelonExp] 		= strval(result);
		cache_get_field_content(0, "pFelonLevel", result); 		ThiefJobInfo[playerid][pFelonLevel] 		= strval(result);
		cache_get_field_content(0, "pRobPersonLimit", result);	ThiefJobInfo[playerid][pRobPersonLimit] 	= strval(result);
		cache_get_field_content(0, "pRobLastVictimPID", result);ThiefJobInfo[playerid][pRobLastVictimPID] = strval(result);
		cache_get_field_content(0, "pTheftLastVictimPID", result);ThiefJobInfo[playerid][pTheftLastVictimPID] = strval(result);
		cache_get_field_content(0, "pTheftPersonLimit", result);ThiefJobInfo[playerid][pTheftPersonLimit] = strval(result);
		cache_get_field_content(0, "pRobHouseLimit", result); 	ThiefJobInfo[playerid][pRobHouseLimit] 	= strval(result);
		cache_get_field_content(0, "pRob2HouseLimit", result); 	ThiefJobInfo[playerid][pRob2HouseLimit] 	= strval(result);
		cache_get_field_content(0, "pRob247Limit", result); 	ThiefJobInfo[playerid][pRob247Limit] 		= strval(result);
		cache_get_field_content(0, "pTheft247Limit", result); 	ThiefJobInfo[playerid][pTheft247Limit] 	= strval(result);
		cache_get_field_content(0, "pForceDoorLimit", result);	ThiefJobInfo[playerid][pForceDoorLimit] 	= strval(result);
		cache_get_field_content(0, "pForceEngineLimit", result);ThiefJobInfo[playerid][pForceEngineLimit] 	= strval(result);
		cache_get_field_content(0, "pDesarmCarLimit", result);	ThiefJobInfo[playerid][pDesarmCarLimit] 	= strval(result);
	}
}

stock createThiefJob(playerid)
{
	new query[128];
	
	if(ThiefJobInfo[playerid][pFelonLevel] == 0) // Si nunca tuvo el job, le seteamos el primer nivel y creamos el registro. Si ya alguna vez lo tuvo (!= 0), no seteamos nada porque ya lo pre-carg� cuando se conect�.
	{
		ThiefJobInfo[playerid][pFelonLevel] = 1;
		format(query,sizeof(query),"INSERT INTO `thief_job` (`accountid`) VALUES ('%d')", PlayerInfo[playerid][pID]);
		#if defined _isamp_debug
		printf("\nCreating thiefJob for player sqlid=%d", PlayerInfo[playerid][pID]);
		printf("\nQuery is: %s",query);
		#endif
		mysql_function_query(dbHandle, query, false, "", "");
	}
}

forward saveThiefJob(playerid);
public saveThiefJob(playerid)
{
	new query[512];
	
	format(query,sizeof(query), "UPDATE thief_job set pFelonExp=%d,pFelonLevel=%d,pRobPersonLimit=%d,pRobLastVictimPID=%d,pTheftLastVictimPID=%d,pTheftPersonLimit=%d,pRob247Limit=%d,pTheft247Limit=%d,pRobHouseLimit=%d,pRob2HouseLimit=%d,pForceDoorLimit=%d,pForceEngineLimit=%d,pDesarmCarLimit=%d WHERE `accountid`=%d",
            ThiefJobInfo[playerid][pFelonExp],
			ThiefJobInfo[playerid][pFelonLevel],
			ThiefJobInfo[playerid][pRobPersonLimit],
			ThiefJobInfo[playerid][pRobLastVictimPID],
			ThiefJobInfo[playerid][pTheftLastVictimPID],
			ThiefJobInfo[playerid][pTheftPersonLimit],
			ThiefJobInfo[playerid][pRob247Limit],
			ThiefJobInfo[playerid][pTheft247Limit],
			ThiefJobInfo[playerid][pRobHouseLimit],
			ThiefJobInfo[playerid][pRob2HouseLimit],
			ThiefJobInfo[playerid][pForceDoorLimit],
			ThiefJobInfo[playerid][pForceEngineLimit],
			ThiefJobInfo[playerid][pDesarmCarLimit],
			getPlayerMysqlId(playerid));

	mysql_function_query(dbHandle, query, false, "", "");
	return 1;
}

forward theftTimer(playerid, type, biz);
public theftTimer(playerid, type, biz)
{
	new
		felonLevel = ThiefJobInfo[playerid][pFelonLevel],
	    string[128];
	    
	if(type == 0) {
		if(theftTime[playerid] > 0) {
		    if(theftTime[playerid] == policeCallTime[playerid]) {
				new
					rnd;

		       	if(felonLevel >= 6) {
				    rnd = random(3);
				} else if(felonLevel == 1) {
				    rnd = random(4);
				} else if(felonLevel == 2) {
					rnd = random(3);
				} else if(felonLevel >= 3) {
				    rnd = random(2);
				}
				if(rnd == 0 || (felonLevel >= 6 && rnd == 1)) {
				} else {
				    robberyAlert(playerid,"hurto en negocio","an�nimo",Business[biz][bName]);	
					
			        SendClientMessage(playerid, COLOR_WHITE, "�Un empleado ha notado tu accionar y ha llamado a la polic�a!");
			        SendClientMessage(playerid, COLOR_WHITE, "Puedes utilizar /correr para escapar dejando toda la mercanc�a o esperar para terminar.");
				}
			}
	 		format(string, sizeof(string), "~w~Tomando mercancia   ~r~%d~w~ segundos", theftTime[playerid]);
	 		GameTextForPlayer(playerid, string, 1000, 4);
	        theftTime[playerid]--;
		} else if(theftTime[playerid] == 0) {
			new takeMoney = 50 + random(200);
		    theftTime[playerid] = -1;
			SendFMessage(playerid, COLOR_WHITE, "Has tomado la mercanc�a por un valor de $%d, �ret�rate de la tienda!", takeMoney);
			TogglePlayerControllable(playerid, true);
			GivePlayerCash(playerid, takeMoney);
			if(ThiefJobInfo[playerid][pFelonLevel] == 2) {
				GiveJobExp(playerid, JOB_FELON, 6);
			}
			SetPVarInt(playerid, "disabled", DISABLE_NONE);
			KillTimer(GetPVarInt(playerid, "theftTimer"));
			printf("%s hurto tienda", GetPlayerNameEx(playerid));
		}
		
	} else if(type == 1) {
	    if(theftTime[playerid] > 0) {
		    if(theftTime[playerid] == policeCallTime[playerid]) {
				
				robberyAlert(playerid,"robo a mano armada","an�nimo",Business[biz][bName]);				

		        SendClientMessage(playerid, COLOR_WHITE, "�Un empleado ha notado tu accionar y ha llamado a la polic�a!");
		        SendClientMessage(playerid, COLOR_WHITE, "Puedes utilizar /correr para escapar dejando todo el dinero o esperar para terminar.");
		    }
		    format(string, sizeof(string), "~w~Guardando dinero ~r~%d~w~ segundos", theftTime[playerid]);
	 		GameTextForPlayer(playerid, string, 1000, 4);
	        theftTime[playerid]--;
        } else if(theftTime[playerid] == 0) {
			new takeMoney = 400 + random(400);
            theftTime[playerid] = -1;
			SendFMessage(playerid, COLOR_WHITE, "Has robado $%d de la caja, �escapa antes de que venga la polic�a!", takeMoney);
			TogglePlayerControllable(playerid, true);
			GivePlayerCash(playerid, takeMoney);
			if(ThiefJobInfo[playerid][pFelonLevel] == 4) {
		 		GiveJobExp(playerid, PlayerInfo[playerid][pJob], 5);
		 	}
			DeletePVar(playerid, "stealAmount");
			SetPVarInt(playerid, "disabled", DISABLE_NONE);
			KillTimer(GetPVarInt(playerid, "theftTimer"));
			printf("%s asalto tienda", GetPlayerNameEx(playerid));
        }
        
	} else if(type == 2){
	    if(theftTime[playerid] > 0) {
	        if(theftTime[playerid] == policeCallTime[playerid]) {
			    		
				robberyAlert(playerid,"hurto en domicilio particular","an�nimo","",biz);
				
		        SendClientMessage(playerid, COLOR_WHITE, "�Un vecino ha notado tu entrada forzosa y ha llamado a la polic�a!");
		        SendClientMessage(playerid, COLOR_WHITE, "Puedes utilizar /correr para escapar dejando la bolsa de objetos o esperar para terminar.");
		    }
		    format(string, sizeof(string), "~w~Robando objetos ~r~%d~w~ segundos", theftTime[playerid]);
	 		GameTextForPlayer(playerid, string, 1000, 4);
	        theftTime[playerid]--;
        } else if(theftTime[playerid] == 0) {
			new takeMoney = 800 + random(400);
            theftTime[playerid] = -1;
			SendFMessage(playerid, COLOR_WHITE, "Has robado $%d objetos de valor, �escapa antes de que venga la polic�a!", takeMoney);
			TogglePlayerControllable(playerid, true);
			GivePlayerCash(playerid, takeMoney);
			if(ThiefJobInfo[playerid][pFelonLevel] == 5) {
		 		GiveJobExp(playerid, JOB_FELON, 6);
		 	}
			DeletePVar(playerid, "stealAmount");
			SetPVarInt(playerid, "disabled", DISABLE_NONE);
			KillTimer(GetPVarInt(playerid, "theftTimer"));
			printf("%s hurto casa", GetPlayerNameEx(playerid));
        }
        
	} else if(type == 3){
	    if(theftTime[playerid] > 0) {
	        if(theftTime[playerid] == policeCallTime[playerid]) {
			    		
				robberyAlert(playerid,"asalto en domicilio particular","an�nimo","",biz);
				
		        SendClientMessage(playerid, COLOR_WHITE, "�Un vecino ha notado tu entrada forzosa y ha llamado a la polic�a!");
		        SendClientMessage(playerid, COLOR_WHITE, "Puedes utilizar /correr para escapar dejando el dinero o esperar para terminar.");
		    }
		    format(string, sizeof(string), "~w~Buscando dinero ~r~%d~w~ segundos", theftTime[playerid]);
	 		GameTextForPlayer(playerid, string, 1000, 4);
	        theftTime[playerid]--;
        } else if(theftTime[playerid] == 0) {
			new takeMoney = 1200 + random(600);
            theftTime[playerid] = -1;
			SendFMessage(playerid, COLOR_WHITE, "Has robado $%d pesos, �escapa antes de que venga la polic�a!", takeMoney);
			TogglePlayerControllable(playerid, true);
			GivePlayerCash(playerid, takeMoney);
			if(ThiefJobInfo[playerid][pFelonLevel] == 6) {
		 		GiveJobExp(playerid, JOB_FELON, 7);
		 	}
			DeletePVar(playerid, "stealAmount");
			SetPVarInt(playerid, "disabled", DISABLE_NONE);
			KillTimer(GetPVarInt(playerid, "theftTimer"));
			printf("%s asalto casa", GetPlayerNameEx(playerid));
        }
	}
	return 1;
}

stock robberyAlert(playerid, accusedOf[], accusedBy[], propName[], houseId = 0)
{
	new string[128];
	
 	if(!usingMask[playerid]) // Si roba sin mascara le damos sosp
 	{
		if(GetPlayerWantedLevelEx(playerid) < 2)
			SetPlayerWantedLevelEx(playerid, 2);
		else
			SetPlayerWantedLevelEx(playerid, GetPlayerWantedLevelEx(playerid) + 1);
	}
	
	if(houseId){
	    format(string, sizeof(string), "[Dpto. de polic�a]: un vecino ha reportado una intrusi�n en la casa Nro %d. Marcamos la direcci�n en su GPS.", houseId);
		foreach(new play : Player)
		{
			if(PlayerInfo[play][pFaction] == FAC_PMA && CopDuty[play])
			{
			    SendClientMessage(play, COLOR_PMA, string);
				SetPlayerCheckpoint(play, House[houseId][OutsideX], House[houseId][OutsideY], House[houseId][OutsideZ], 2.0);
			}
		}
	} else {
		format(string, sizeof(string), "[Dpto. de polic�a]: un civil ha reportado un sospechoso por %s en %s.", accusedOf, propName);
		foreach(new play : Player)
		{
			if(PlayerInfo[play][pFaction] == FAC_PMA && CopDuty[play])
			    SendClientMessage(play, COLOR_PMA, string);
		}
	}
	
	format(PlayerInfo[playerid][pAccusedOf], 64, accusedOf);
	format(PlayerInfo[playerid][pAccusedBy], 24, accusedBy);

}

public robberyCancel(playerid)
{
    SendClientMessage(playerid, COLOR_YELLOW2, "El robo ha sido cancelado porque no respondiste en 30 segundos.");
    SendClientMessage(felon[playerid], COLOR_YELLOW2, "El robo ha sido cancelado porque la victima no ha respondido en 30 segundos.");
    felon[playerid] = INVALID_PLAYER_ID;
	return 1;
}

public GiveJobExp(playerid, job, exp)
{
    if(job == JOB_FELON)
	{
		if(ThiefJobInfo[playerid][pFelonExp] + exp >= GetJobExpNeeded(playerid, job))
		{
			ThiefJobInfo[playerid][pFelonLevel]++;
			ThiefJobInfo[playerid][pFelonExp] = 0;
			SendClientMessage(playerid, COLOR_LIGHTBLUE, "�Ahora eres un delincuente m�s experimentado y tienes acceso a nuevos comandos!");
			PlayerPlaySound(playerid, 1052, 0.0, 0.0, 0.0);
		} else
			ThiefJobInfo[playerid][pFelonExp] += exp;
	}
	return 1;
}

stock GetJobExpNeeded(playerid, job)
{
	if(job == JOB_FELON)
	{
        new
			level = ThiefJobInfo[playerid][pFelonLevel],
			result;
			
        result = (level * level + 15) * (JOB_EXP_MULTIPLIER + level);
		return result;
	}
	return 0;
}

stock updateThiefCounters(playerid)
{
	if(ThiefJobInfo[playerid][pRobPersonLimit] > 0)
		ThiefJobInfo[playerid][pRobPersonLimit]--;
		
	if(ThiefJobInfo[playerid][pTheftPersonLimit] > 0)
		ThiefJobInfo[playerid][pTheftPersonLimit]--;
		
	if(ThiefJobInfo[playerid][pRob247Limit] > 0)
		ThiefJobInfo[playerid][pRob247Limit]--;
		
	if(ThiefJobInfo[playerid][pRobHouseLimit] > 0)
		ThiefJobInfo[playerid][pRobHouseLimit]--;
		
	if(ThiefJobInfo[playerid][pRob2HouseLimit] > 0)
	    ThiefJobInfo[playerid][pRob2HouseLimit]--;
	    
	if(ThiefJobInfo[playerid][pTheft247Limit] > 0)
		ThiefJobInfo[playerid][pTheft247Limit]--;

	if(ThiefJobInfo[playerid][pForceDoorLimit] > 0)
	    ThiefJobInfo[playerid][pForceDoorLimit]--;
	    
	if(ThiefJobInfo[playerid][pForceEngineLimit] > 0)
	    ThiefJobInfo[playerid][pForceEngineLimit]--;
	    
	if(ThiefJobInfo[playerid][pDesarmCarLimit] > 0)
	    ThiefJobInfo[playerid][pDesarmCarLimit]--;
}

//=========================COMANDOS DE LADRON===================================

CMD:asaltar(playerid, params[])
{
	new
	    targetMoney,
		randomAmount,
	    string[128],
		target;

	if(PlayerInfo[playerid][pJob] != JOB_FELON)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener el trabajo de delincuente para utilizar este comando.");
	if(ThiefJobInfo[playerid][pFelonLevel] < 3)
		return SendClientMessage(playerid, COLOR_YELLOW2, "No tienes la experiencia necesaria para poder asaltar.");
    if(isPlayerJailed(playerid) > 0)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo estando encarcelado.");
	if(sscanf(params, "u", target)) {
        SendClientMessage(playerid, COLOR_GREY, "{5CCAF1}[Sintaxis]:{C8C8C8} /asaltar [IDJugador/ParteDelNombre]");
        SendClientMessage(playerid, COLOR_WHITE, "Info: si la v�ctima se encuentra agonizando no obtendr�s experiencia.");
		return 1;
	}
	if(target == playerid)
        return SendClientMessage(playerid, COLOR_YELLOW2, "No te puedes hurtar a t� mismo.");
	if(ThiefJobInfo[playerid][pRobPersonLimit] != 0)
	{
	    SendFMessage(playerid, COLOR_YELLOW2, "Debes esperar %d minutos antes de volver a robar.", ThiefJobInfo[playerid][pRobPersonLimit] / 60);
	    return 1;
	}
    if(target == INVALID_PLAYER_ID)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "Jugador incorrecto.");
	if(PlayerInfo[playerid][pFaction] != 0 && PlayerInfo[target][pFaction] == PlayerInfo[playerid][pFaction])
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No le puedes robar a esta persona.");
	if(!hasFireGun(playerid))
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener un arma de fuego en la mano.");
	if(ThiefJobInfo[playerid][pRobLastVictimPID] == getPlayerMysqlId(target) && !AdminDuty[playerid])
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No le puedes robar dos veces seguidas a la misma persona.");
  	if(GetDistanceBetweenPlayers(playerid, target) > 4.0)
  		return SendClientMessage(playerid, COLOR_YELLOW2, "El sujeto se encuentra demasiado lejos.");
  	if(felon[target] != INVALID_PLAYER_ID)
  	    return SendClientMessage(playerid, COLOR_YELLOW2, "El sujeto ya esta siendo robado.");
    if(PlayerInfo[target][pLevel] < 3)
        return SendClientMessage(playerid, COLOR_YELLOW2, "No le puedes robar a un personaje menor de nivel 3.");

	targetMoney = GetPlayerCash(target);
	if(targetMoney <= 0) {
		format(string, sizeof(string), "intenta robarle algo de dinero a %s pero al parecer no tiene nada.", GetPlayerNameEx(target));
		PlayerActionMessage(playerid, 15.0, string);
		return 1;
	}
    if(GetPVarInt(target, "disabled") != DISABLE_DYING && GetPVarInt(target, "disabled") != DISABLE_DEATHBED) {
        SendFMessage(playerid, COLOR_WHITE, "Has intentado robarle a %s, espera la reacci�n del sujeto...", GetPlayerNameEx(target));
        SendClientMessage(target, COLOR_WHITE, "Te est�n intentando robar, puedes '/resistirte', '/cooperar' o '/mentir'.");
        SendClientMessage(target, COLOR_WHITE, "Recuerda que si el ladr�n te descubre mintiendo se enfadar� y perder�s m�s dinero.");
        felon[target] = playerid;
        SetPVarInt(playerid, "robberyCancel", SetTimerEx("robberyCancel", 30000, false, "i", target));
	} else
		{
	        if(targetMoney <= 100)
	            randomAmount = targetMoney;
			else
	  			randomAmount = (targetMoney/2) + random(targetMoney/2) + 1; // Obtendr� si o si la mitad del dinero del jugador, m�s un valor aleatorio entre 0 y la totalidad de la otra mitad.
	        // Ponemos un l�mite para volver a robar de 15m.
			ThiefJobInfo[playerid][pRobPersonLimit] = 60 * (15 - random(5));
	  		format(string, sizeof(string), "le ha robado algo de dinero a %s tomando provecho de su incapacidad.", GetPlayerNameEx(target));
	 		PlayerActionMessage(playerid, 15.0, string);
	        printf("%s asalto agonizando a %s", GetPlayerNameEx(playerid), GetPlayerNameEx(target));
	        if(randomAmount > 50000)
	            randomAmount = 50000;
			GivePlayerCash(playerid, randomAmount);
			GivePlayerCash(target, -randomAmount);
			if(GetPVarInt(target, "disabled") != DISABLE_DYING && GetPVarInt(target, "disabled") != DISABLE_DEATHBED && ThiefJobInfo[playerid][pFelonLevel] == 3) {
		 		GiveJobExp(playerid, PlayerInfo[playerid][pJob], 5);
		 	}
			ThiefJobInfo[playerid][pRobLastVictimPID] = getPlayerMysqlId(target);
		}
	if(random(2) == 1) {
		if(GetPlayerWantedLevelEx(playerid) < 2) {
       		SetPlayerWantedLevelEx(playerid, 2);
		} else {
			SetPlayerWantedLevelEx(playerid, GetPlayerWantedLevelEx(playerid) + 1);
		}
		format(string, sizeof(string), "[Dpto. de polic�a]: un civil ha reportado a %s como sospechoso de robo a mano armada.", GetPlayerNameEx(playerid));
		SendFactionMessage(FAC_PMA, COLOR_PMA, string);
		format(PlayerInfo[playerid][pAccusedOf], 64, "robo a mano armada");
		format(PlayerInfo[playerid][pAccusedBy], 24, "an�nimo");
    }
	return 1;
}

CMD:resistirte(playerid, params[])
{
	if(felon[playerid] != INVALID_PLAYER_ID)
	{
		PlayerActionMessage(playerid, 15.0, "se resiste al robo.");
		SendClientMessage(felon[playerid], COLOR_WHITE, "La victima ha rechazado darte del dinero.");
		KillTimer(GetPVarInt(felon[playerid], "robberyCancel"));
		felon[playerid] = INVALID_PLAYER_ID;
	}
	return 1;
}

CMD:mentir(playerid, params[])
{
	new
	    string[128],
	    targetMoney = GetPlayerCash(playerid);
	    
	if(felon[playerid] != INVALID_PLAYER_ID)
	{
	    if(random(1 + ThiefJobInfo[felon[playerid]][pFelonLevel]) == 0)
		{
			format(string, sizeof(string), "intenta robarle algo de dinero a %s pero al parecer no tiene nada.", GetPlayerNameEx(playerid));
			PlayerActionMessage(felon[playerid], 15.0, string);
			SendClientMessage(playerid, COLOR_WHITE, "Has logrado enga�ar al delincuente y no has perdido dinero.");
	    } else {
	        PlayerActionMessage(playerid, 15.0, "ha sido descubierto intentando enga�ar al ladr�n.");
            format(string, sizeof(string), "le ha robado algo de dinero a %s.", GetPlayerNameEx(playerid));
        	PlayerActionMessage(felon[playerid], 15.0, string);
        	if(targetMoney > 50000)
        	    targetMoney = 50000;
			GivePlayerCash(felon[playerid], targetMoney);
			GivePlayerCash(playerid, -targetMoney);
	   		if(ThiefJobInfo[felon[playerid]][pFelonLevel] == 3) {
		 		GiveJobExp(felon[playerid], PlayerInfo[felon[playerid]][pJob], 5);
		 	}
			printf("%s asalto a %s", GetPlayerNameEx(felon[playerid]), GetPlayerNameEx(playerid));
	    }
	    ThiefJobInfo[felon[playerid]][pRobLastVictimPID] = getPlayerMysqlId(playerid);
	    ThiefJobInfo[felon[playerid]][pRobPersonLimit] = 60 * (15 - random(5));
	    KillTimer(GetPVarInt(felon[playerid], "robberyCancel"));
	    felon[playerid] = INVALID_PLAYER_ID;
	}
	return 1;
}

CMD:cooperar(playerid, params[])
{
	new
	    string[128],
	    targetMoney = GetPlayerCash(playerid),
		randomAmount = targetMoney / 2 + random(targetMoney / 2) + 1;

	if(felon[playerid] != INVALID_PLAYER_ID)
	{
	    if(targetMoney > 1)
		{
			format(string, sizeof(string), "le ha robado algo de dinero a %s.", GetPlayerNameEx(playerid));
			PlayerActionMessage(felon[playerid], 15.0, string);
			SendClientMessage(playerid, COLOR_YELLOW2, "Has cooperado y el ladr�n te ha robado algo de dinero.");
			if(randomAmount > 50000)
			    randomAmount = 50000;
			GivePlayerCash(felon[playerid], randomAmount);
			GivePlayerCash(playerid, -randomAmount);
			if(ThiefJobInfo[felon[playerid]][pFelonLevel] == 3)
		 		GiveJobExp(felon[playerid], PlayerInfo[felon[playerid]][pJob], 5);
		} else {
			format(string, sizeof(string), "intenta robarle algo de dinero a %s pero al parecer no tiene nada.", GetPlayerNameEx(playerid));
			PlayerActionMessage(felon[playerid], 15.0, string);
		}
		printf("%s asalto a %s", GetPlayerNameEx(felon[playerid]), GetPlayerNameEx(playerid));
		ThiefJobInfo[felon[playerid]][pRobPersonLimit] = 60 * (15 - random(5));
		ThiefJobInfo[felon[playerid]][pRobLastVictimPID] = getPlayerMysqlId(playerid);
		KillTimer(GetPVarInt(felon[playerid], "robberyCancel"));
		felon[playerid] = INVALID_PLAYER_ID;
	}
	return 1;
}

CMD:asaltartienda(playerid, params[])
{
	new business = GetPlayerBusiness(playerid);
	    
	if(PlayerInfo[playerid][pJob] != JOB_FELON)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener el trabajo de delincuente para utilizar este comando.");
	if(ThiefJobInfo[playerid][pFelonLevel] < 4)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Necesitas m�s experiencia para utilizar este comando.");
	if(ThiefJobInfo[playerid][pRob247Limit] != 0)
	{
	    SendFMessage(playerid, COLOR_YELLOW2, "Debes esperar %d minutos antes de volver a asaltar.", ThiefJobInfo[playerid][pRob247Limit] / 60);
	    return 1;
	}
	if(business > 0 && business == PlayerInfo[playerid][pBizKey])
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes asaltar tu propio negocio.");
    if(Business[business][bType] != BIZ_247 && Business[business][bType] != BIZ_CLOT && Business[business][bType] != BIZ_CLOT2 && Business[business][bType] != BIZ_CLUB && Business[business][bType] != BIZ_CLUB2 && Business[business][bType] != BIZ_REST && Business[business][bType] != BIZ_HARD && Business[business][bType] != BIZ_PIZZERIA && Business[business][bType] != BIZ_BURGER1 && Business[business][bType] != BIZ_BURGER2 && Business[business][bType] != BIZ_BELL)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes estar en un 24-7 / Local de ropa / Bar / Discoteca o Club Nocturno / Restaurant / Comidas r�pidas.");
	if(!hasFireGun(playerid))
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener un arma de fuego en la mano.");

	ApplyAnimation(playerid, "SHOP", "ROB_Loop_Threat", 4.0, 0, 0, 0, 0, 0);
	PlayerActionMessage(playerid, 15.0, "apunta al empleado con el arma y le hace una se�a para que le de todo el dinero.");
	policeCallTime[playerid] = 20 + random(35);
	SetPVarInt(playerid, "theftTimer", SetTimerEx("theftTimer", 1000, true, "iii", playerid, 1, business));
	SetPVarInt(playerid, "disabled", DISABLE_STEALING);
	theftTime[playerid] = 60;
	ThiefJobInfo[playerid][pRob247Limit] = 60 * 60;
	return 1;
}

CMD:asaltarcasa(playerid, params[])
{
    new house = GetPlayerHouse(playerid);

	if(PlayerInfo[playerid][pJob] != JOB_FELON)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener el trabajo de delincuente para utilizar este comando.");
	if(!hasFireGun(playerid))
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener un arma de fuego en la mano.");
	if(ThiefJobInfo[playerid][pFelonLevel] < 6)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Necesitas m�s experiencia para utilizar este comando.");
	if(ThiefJobInfo[playerid][pRob2HouseLimit] != 0)
 	{
	    SendFMessage(playerid, COLOR_YELLOW2, "Debes esperar %d minutos antes de volver a asaltar.", ThiefJobInfo[playerid][pRob2HouseLimit] / 60);
	    return 1;
	}
	if(house == 0)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes estar en una casa.");
 	if(house == PlayerInfo[playerid][pHouseKey])
		return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes robar tu propia casa!");

	PlayerActionMessage(playerid, 15.0, "comienza a buscar dinero por la casa y lo guarda en una bolsa.");
	policeCallTime[playerid] = 20 + random(60);
	SetPVarInt(playerid, "theftTimer", SetTimerEx("theftTimer", 1000, true, "iii", playerid, 3, house));
	SetPVarInt(playerid, "disabled", DISABLE_STEALING);
	theftTime[playerid] = 100;
	ThiefJobInfo[playerid][pRob2HouseLimit] = 50 * 60;
	return 1;
}
CMD:hurtarcasa(playerid, params[])
{
	new house = GetPlayerHouse(playerid);

	if(PlayerInfo[playerid][pJob] != JOB_FELON)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener el trabajo de delincuente para utilizar este comando.");
	if(ThiefJobInfo[playerid][pFelonLevel] < 5)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Necesitas m�s experiencia para utilizar este comando.");
	if(ThiefJobInfo[playerid][pRobHouseLimit] != 0)
 	{
	    SendFMessage(playerid, COLOR_YELLOW2, "Debes esperar %d minutos antes de volver a hurtar.", ThiefJobInfo[playerid][pRobHouseLimit] / 60);
	    return 1;
	}
	if(house == 0)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes estar en una casa.");
 	if(house == PlayerInfo[playerid][pHouseKey])
		return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes robar tu propia casa!");

	PlayerActionMessage(playerid, 15.0, "empieza a buscar objetos de valor y los almacena en una bolsa.");
	policeCallTime[playerid] = 20 + random(60);
	SetPVarInt(playerid, "theftTimer", SetTimerEx("theftTimer", 1000, true, "iii", playerid, 2, house));
	SetPVarInt(playerid, "disabled", DISABLE_STEALING);
	theftTime[playerid] = 100;
	ThiefJobInfo[playerid][pRobHouseLimit] = 40 * 60;
	return 1;
}

CMD:hurtartienda(playerid, params[])
{
	new business = GetPlayerBusiness(playerid);
	    
	if(PlayerInfo[playerid][pJob] != JOB_FELON)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener el trabajo de delincuente para utilizar este comando.");
	if(ThiefJobInfo[playerid][pFelonLevel] < 2)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Necesitas m�s experiencia para utilizar este comando.");
	if(ThiefJobInfo[playerid][pTheft247Limit] != 0)
 	{
	    SendFMessage(playerid, COLOR_YELLOW2, "Debes esperar %d minutos antes de volver a hurtar.", ThiefJobInfo[playerid][pTheft247Limit] / 60);
	    return 1;
	}
	if(business > 0 && business == PlayerInfo[playerid][pBizKey])
 		return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hurtar tu propio negocio.");
    if(Business[business][bType] != BIZ_247 && Business[business][bType] != BIZ_CLOT && Business[business][bType] != BIZ_CLOT2)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes estar en un 24-7 o en una tienda de ropa.");
		
    ThiefJobInfo[playerid][pTheft247Limit] = 60 * 30;
	PlayerActionMessage(playerid, 15.0, "comienza a tomar mercanc�a y guardarla debajo de su ropa.");
	theftTime[playerid] = 60;
	SetPVarInt(playerid, "disabled", DISABLE_STEALING);
	policeCallTime[playerid] = 20 + random(35);
	SetPVarInt(playerid, "theftTimer", SetTimerEx("theftTimer", 1000, true, "iii", playerid, 0, business));
	return 1;
}

CMD:correr(playerid, params[])
{
    if(PlayerInfo[playerid][pJob] != JOB_FELON)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener el trabajo de delincuente para utilizar este comando.");
	if(GetPVarInt(playerid, "disabled") != DISABLE_STEALING)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "Debes estar robando para utilizar este comando.");
	    
	TogglePlayerControllable(playerid, true);
	SetPVarInt(playerid, "disabled", DISABLE_NONE);
    theftTime[playerid] = -1;
    DeletePVar(playerid, "stealAmount");
	KillTimer(GetPVarInt(playerid, "theftTimer"));
	PlayerActionMessage(playerid, 15.0, "arroja al suelo todo el dinero/mercanc�a robado.");
	return 1;
}

CMD:hurtar(playerid, params[])
{
	new rnd, string[128], target, targetMoney, takeMoney;

	if(PlayerInfo[playerid][pJob] != JOB_FELON)
		return SendClientMessage(playerid, COLOR_YELLOW2, "Debes tener el trabajo de delincuente para utilizar este comando.");
 	if(ThiefJobInfo[playerid][pFelonLevel] < 1)
		return SendClientMessage(playerid, COLOR_YELLOW2, "[BUG] Tendr�as que empezar el job con nivel 1 de delincuente, rep�rtalo a un administrador.");
    if(PlayerInfo[playerid][pJailed] > 0)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo estando encarcelado.");
	if(IsPlayerInAnyVehicle(playerid) && GetVehicleType(GetPlayerVehicleID(playerid)) != VTYPE_BIKE)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo desde un veh�culo.");
	if(sscanf(params, "u", target)) {
        SendClientMessage(playerid, COLOR_GREY, "{5CCAF1}[Sintaxis]:{C8C8C8} /hurtar [ID/Jugador]");
        SendClientMessage(playerid, COLOR_WHITE, "Info: si la v�ctima se encuentra agonizando no obtendr�s experiencia.");
		return 1;
	}
	if(target == playerid)
        return SendClientMessage(playerid, COLOR_YELLOW2, "No te puedes hurtar a t� mismo.");
	if(ThiefJobInfo[playerid][pTheftPersonLimit] > 0) {
	    SendFMessage(playerid, COLOR_YELLOW2, "Has superado el l�mite de hurtos, espera %d minutos.", ThiefJobInfo[playerid][pTheftPersonLimit] / 60);
	    return 1;
	}
    if(target == INVALID_PLAYER_ID)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "Jugador incorrecto.");
	if(PlayerInfo[playerid][pFaction] != 0 && PlayerInfo[target][pFaction] == PlayerInfo[playerid][pFaction])
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No le puedes robar a esta persona.");
  	if(GetDistanceBetweenPlayers(playerid, target) > 1.2)
  		return SendClientMessage(playerid, COLOR_YELLOW2, "�Recuerda que debes estar cerca del bolsillo/cartera de la v�ctima!");
  	if(felon[target] != INVALID_PLAYER_ID)
  	    return SendClientMessage(playerid, COLOR_YELLOW2, "El sujeto ya esta siendo robado.");
    if(PlayerInfo[target][pLevel] < 3)
        return SendClientMessage(playerid, COLOR_YELLOW2, "No le puedes robar a un personaje menor de nivel 3.");
	if(ThiefJobInfo[playerid][pTheftLastVictimPID] == getPlayerMysqlId(target))
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No le puedes hurtar dos veces seguidas a la misma persona.");
 	targetMoney = GetPlayerCash(target);
    if(targetMoney <= 0)
        return SendClientMessage(playerid, COLOR_YELLOW2, "El sujeto no tiene nada de valor.");

	rnd = random(3);
	if(rnd == 0) // 33 % de que falle.
	{
		format(string, sizeof(string), "ha realizado unas maniobras para hurtar algo del bolsillo de %s pero es descubierto.", GetPlayerNameEx(target));
        PlayerActionMessage(playerid, 15.0, string);
        SendClientMessage(playerid, COLOR_WHITE, "�Has fallado y el sujeto lo ha notado, mejor comienza a correr!");
	} else
	    {
			takeMoney = 30 + random(20);
			if(takeMoney > targetMoney)
   				takeMoney = targetMoney;
		    SendFMessage(playerid, COLOR_WHITE, "Has tomado $%d del sujeto sin que �ste se percate de lo sucedido, act�a como si nada hubiera pasado.", takeMoney);
		    SendClientMessage(target, COLOR_WHITE, "[AVISO OOC]: Un sujeto te ha hurtado sin que te des cuenta. Esta informaci�n NO la puedes usar IC.");
			GivePlayerCash(target, -takeMoney);
			GivePlayerCash(playerid, takeMoney);
		}
 	if(GetPVarInt(target, "disabled") != DISABLE_DYING && GetPVarInt(target, "disabled") != DISABLE_DEATHBED && ThiefJobInfo[playerid][pFelonLevel] == 1) {
 		GiveJobExp(playerid, PlayerInfo[playerid][pJob], 1);
 	}
	ThiefJobInfo[playerid][pTheftPersonLimit] = 600;
	ThiefJobInfo[playerid][pTheftLastVictimPID] = getPlayerMysqlId(target);
	printf("%s hurto a %s", GetPlayerNameEx(playerid), GetPlayerNameEx(target));
	return 1;
}

CMD:verexp(playerid, params[])
{
	if(ThiefJobInfo[playerid][pFelonLevel] > 0)
	{
	    SendClientMessage(playerid, COLOR_WHITE, "Experiencia:");
		SendFMessage(playerid, COLOR_WHITE, "* Delincuente - nivel: %d exp: %d/%d", ThiefJobInfo[playerid][pFelonLevel], ThiefJobInfo[playerid][pFelonExp], GetJobExpNeeded(playerid, JOB_FELON));
	}
	return 1;
}

CMD:delincuenteayuda(playerid, params[]) {
	if(PlayerInfo[playerid][pJob] != JOB_FELON)
	    return 1;
	SendClientMessage(playerid, COLOR_WHITE, "[Ladron]: /hurtar /hurtartienda /asaltar /asaltartienda /hurtarcasa /asaltarcasa /verexp");
	SendClientMessage(playerid, COLOR_WHITE, "[Ladron de bancos]: /grupoayuda");
    SendClientMessage(playerid, COLOR_WHITE, "[Ladron de veh�culos]: /barreta /puente /desarmar");
	SendClientMessage(playerid, COLOR_WHITE, "Info: al robar con un comando de tu nivel obtendr�s experiencia para subir de nivel y desbloquear nuevos comandos.");
	return 1;
}
