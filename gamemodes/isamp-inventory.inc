#if defined _isamp_inventory_included
	#endinput
#endif
#define _isamp_inventory_included

//=====================SISTEMA DE INVENTARIO DE JUGADORES=======================

//================================CONSTANTES====================================

#define INV_MAX_SLOTS           2
#define SLOT_TYPE_INV      		3

//============================VARIABLES INTERNAS================================

enum SlotInfo {
	Item,
	Amount,
};

new InvInfo[MAX_PLAYERS][INV_MAX_SLOTS][SlotInfo];

//===============================FUNCIONES======================================

forward GetInvItem(playerid, invslot); // Devuelve el nro de item del slot indicado del inventario del jugador. En caso de error retorna -1.
forward GetInvParam(playerid, invslot); // Devuelve el parametro "cantidad" del slot indicado del inventario del jugador. En caso de error retorna -1.
forward SetInvItemAndParam(playerid, invslot, itemid, param); // Setea item y cantidad en el slot indicado. Guardado instantaneo en la DB.
forward ResetInv(playerid); // Borra todo slot del inventario del jugador y lo resetea a cero (para resetiar cuando se conecta nuevo jugador).
forward ResetAndSaveInv(playerid); // Borra todo slot del inventario del jugador, lo resetea a cero, y lo guarda en DB.
forward SearchInvForItem(playerid, itemid); // Busca un item pasado por parametro dentro de un inventario. Si existe, devuelve el slot en el que est�, sino retorna -1.
forward PrintInvForPlayer(playerid, targetid); // Imprime el inventario del jugador X al jugador Y.
forward SearchInvFreeSlot(playerid); // Busca por el primer slot vacio en el inventario y devuelve su posicion. De no haber espacio retorna -1.
forward LoadInvInfo(playerid); // Carga desde la base de datos al GM en ejecuci�n el inventario del jugador indicado.

//=======================IMPLEMENTACI�N DE FUNCIONES============================

stock GetInvItem(playerid, invslot)
{
	if(invslot >= 0 && invslot < INV_MAX_SLOTS)
		return InvInfo[playerid][invslot][Item];
	else
  	  	return -1;
}

stock GetInvParam(playerid, invslot)
{
	if(invslot >= 0 && invslot < INV_MAX_SLOTS)
		return InvInfo[playerid][invslot][Amount];
	else
		return -1;
}

stock SetInvItemAndParam(playerid, invslot, itemid, param)
{
	if(invslot >= 0 && invslot < INV_MAX_SLOTS)
	{
	    if(GetInvItem(playerid, invslot) > 0) // Si quiere sobreescribir el slot teniendo algo dentro
	    {
			InvInfo[playerid][invslot][Item] = 0;
			InvInfo[playerid][invslot][Amount] = 0;
			SaveSlotInfo(SLOT_TYPE_INV, playerid, invslot); // Borramos el anterior
		}
		if(itemid != 0)
		{
			InvInfo[playerid][invslot][Item] = itemid;
			InvInfo[playerid][invslot][Amount] = param;
			SaveSlotInfo(SLOT_TYPE_INV, playerid, invslot);
		}
	}
	return 1;
}

stock ResetInv(playerid)
{
	for(new i = 0; i < INV_MAX_SLOTS; i++)
	{
		InvInfo[playerid][i][Item] = 0;
		InvInfo[playerid][i][Amount] = 0;
	}
	return 1;
}

stock ResetAndSaveInv(playerid)
{
	for(new i = 0; i < INV_MAX_SLOTS; i++)
	{
	    if(InvInfo[playerid][i][Item] > 0 && GetItemType(InvInfo[playerid][i][Item]) == ITEM_WEAPON)
	    {
			InvInfo[playerid][i][Item] = 0;
			InvInfo[playerid][i][Amount] = 0;
   			SaveSlotInfo(SLOT_TYPE_INV, playerid, i);
		}
	}
	return 1;
}

stock SearchInvFreeSlot(playerid)
{
    for(new i = 0; i < INV_MAX_SLOTS; i++)
    {
        if(InvInfo[playerid][i][Item] == 0)
            return i;
	}
	return -1;
}

stock SearchInvForItem(playerid, itemid)
{
	for(new i = 0; i < INV_MAX_SLOTS; i++)
	{
		if(InvInfo[playerid][i][Item] == itemid)
		    return i;
	}
	return -1;
}

stock PrintInvForPlayer(playerid, targetid)
{
	new itemid;

 	SendClientMessage(targetid, COLOR_WHITE, "======================[Inventario]======================");
	for(new i = 0; i < INV_MAX_SLOTS; i++)
	{
		itemid = GetInvItem(playerid, i);
	    if(itemid > 0)
			SendFMessage(targetid, COLOR_WHITE, " - %d- Item: %s - %s: %d", i, GetItemName(itemid), GetItemParamName(itemid), GetInvParam(playerid, i));
	    else
	        SendFMessage(targetid, COLOR_WHITE, " - %d- Vac�o", i);
	}
	SendClientMessage(targetid, COLOR_WHITE, "=====================================================");
	return 1;
}

stock LoadInvInfo(playerid)
{
	new query[128];

	ResetInv(playerid); // Reseteamos la variable a cero y cargamos la nueva data (porque si no carga nada queda la data del anterior pj con misma id)
	format(query, sizeof(query), "SELECT * FROM `slots_info` WHERE `Type`= %d AND `Id` = %d", SLOT_TYPE_INV, PlayerInfo[playerid][pID]);
	mysql_function_query(dbHandle, query, true, "OnSlotsInfoDataLoad", "ii", SLOT_TYPE_INV, playerid);
	return 1;
}

//================================COMANDOS======================================

CMD:inv(playerid, params[])
{
	cmd_inventario(playerid, params);
	return 1;
}

stock TakeItemFromInv(playerid, playerhand, invslot)
{
	new itemid = GetInvItem(playerid, invslot),
		itemparam = GetInvParam(playerid, invslot),
		str[128];

	if(itemid <= 0)
		return SendClientMessage(playerid, COLOR_YELLOW2, "�Item inv�lido o inexistente!");
	if(GetHandItem(playerid, playerhand) != 0)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes tomar el item ya que tienes la mano ocupada.");

	SetHandItemAndParam(playerid, playerhand, itemid, itemparam); // Creaci�n l�gica y grafica en la mano.
	SetInvItemAndParam(playerid, invslot, 0, 0); // Borrado l�gico del item del inventario.
	format(str, sizeof(str), "toma un/a %s de su inventario.", GetItemName(itemid));
	PlayerActionMessage(playerid, 15.0, str);
	return 1;
}

stock SaveItemOnInv(playerid, playerhand, invslot)
{
 	if(GetPVarInt(playerid, "disabled") != DISABLE_NONE)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo en este momento.");
	if(PlayerInfo[playerid][pHealth] < 25.0)
		return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes guardar un item agonizando.");
	if(GetPVarInt(playerid, "cantSaveItems") == 1)
	    return SendClientMessage(playerid, COLOR_YELLOW2, "�Debes esperar un tiempo antes de volver a interactuar con otro item!");

	new validslot,
		itemid = GetHandItem(playerid, playerhand);

	if(itemid == 0)
		return SendClientMessage(playerid, COLOR_YELLOW2, "No tienes nada en esa mano para guardar.");
	if(itemid == WEAPON_SILENCED && isPlayerUsingTazer(playerid))
	    return SendClientMessage(playerid, COLOR_YELLOW2, "No puedes hacerlo con el tazer.");
	if(!HasItemInvPermission(itemid))
		return SendClientMessage(playerid, COLOR_YELLOW2, "Ese item no se puede guardar en el inventario.");

	if(invslot == -1)
	{
	    validslot = SearchInvFreeSlot(playerid);
		if(validslot == -1)
    		return SendClientMessage(playerid, COLOR_YELLOW2, "�El inventario se encuentra lleno!");
	} else
		{
	        validslot = invslot;
			if(validslot < 0 || validslot >= INV_MAX_SLOTS)
			   	return SendClientMessage(playerid, COLOR_YELLOW2, "Slot inv�lido.");
		   	if(GetInvItem(playerid, validslot) != 0)
		   	    return SendClientMessage(playerid, COLOR_YELLOW2, "Ya tienes un item en ese slot.");
		}

	SetInvItemAndParam(playerid, validslot, itemid, GetHandParam(playerid, playerhand));
	SetHandItemAndParam(playerid, playerhand, 0, 0); // Borrado l�gico y grafico de la mano.
	SendFMessage(playerid, COLOR_WHITE, "Has guardado un/a %s en tu inventario.", GetItemName(itemid));
  	SetPVarInt(playerid, "cantSaveItems", 1);
	SetTimerEx("cantSaveItems", 2000, false, "i", playerid);
	return 1;
}

CMD:inventario(playerid, params[])
{
	new returnid,
		cmd[32];

    if(sscanf(params, "s[32]I(-1)", cmd, returnid))
	{
        SendClientMessage(playerid, COLOR_LIGHTYELLOW2, "{5CCAF1}[Sintaxis]:{C8C8C8} (/inv)entario [comando]");
        SendClientMessage(playerid, COLOR_LIGHTYELLOW2, "{5CCAF1}[Comandos]:{C8C8C8} tomar/tomari [slot] - guardar/guardari [slot (opcional)] - desechar [slot]");
		PrintInvForPlayer(playerid, playerid);
    }
	else
	{
		if(strcmp(cmd, "tomar", true) == 0)
		{
			TakeItemFromInv(playerid, HAND_RIGHT, returnid);
        }
		else if(strcmp(cmd, "tomari", true) == 0)
		{
			TakeItemFromInv(playerid, HAND_LEFT, returnid);
	    }
		else if(strcmp(cmd, "guardar", true) == 0)
		{
			SaveItemOnInv(playerid, HAND_RIGHT, returnid);
	    }
		else if(strcmp(cmd, "guardari", true) == 0)
		{
			SaveItemOnInv(playerid, HAND_LEFT, returnid);
	    }
		else if(strcmp(cmd, "desechar", true) == 0)
		{
			new itemid = GetInvItem(playerid, returnid),
				string[128];
			
	        if(PlayerInfo[playerid][pHealth] < 25.0)
	            return SendClientMessage(playerid, COLOR_LIGHTBLUE, "�No puedes desechar un item agonizando!");
	        if(itemid <= 0)
	            return SendClientMessage(playerid, COLOR_YELLOW2, "�Item inv�lido o inexistente!");
	            
      		format(string, sizeof(string), "ha desechado disimuladamente un/a %s.", GetItemName(itemid));
            PlayerActionMessage(playerid, 8.0, string);
			SetInvItemAndParam(playerid, returnid, 0, 0);
		}
	}
	return 1;
}
