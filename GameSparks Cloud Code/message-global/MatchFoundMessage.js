// ====================================================================================================
//
// Cloud Code for MatchFoundMessage, write your code here to customize the GameSparks platform.
//
// For details of the GameSparks Cloud Code API see https://docs.gamesparks.com/
//
// ====================================================================================================

//Acest script este apelat cand a fost formata o pereche de 2 jucatori prin platforma GameSparks
var msg = Spark.message(null);
msg.setPlayerIds([Spark.getData().participants[0].id]);
msg.setMessageData({"action":"Host"});
msg.send();