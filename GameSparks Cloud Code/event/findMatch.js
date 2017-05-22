// ====================================================================================================
//
// Cloud Code for findMatch, write your code here to customize the GameSparks platform.
//
// For details of the GameSparks Cloud Code API see https://docs.gamesparks.com/
//
// ====================================================================================================

//Acest script este apelat cand jucatorul incepe cautarea pentru alti jucatori print platforma GameSparks
var matchshortCode = Spark.getData().match_shortCode;

if(matchshortCode === "matchRanked")
{
    var matchRequest = new SparkRequests.MatchmakingRequest();
    
    matchRequest.matchShortCode = matchshortCode;
    matchRequest.skill = Spark.getPlayer().getBalance1()
    
    matchRequest.Send();
}
else if(matchshortCode === "matchCasual")
{
    var matchRequest = new SparkRequests.MatchmakingRequest();
    
    matchRequest.matchShortCode = matchshortCode;
    matchRequest.skill = 0;
    
    matchRequest.Send();
}