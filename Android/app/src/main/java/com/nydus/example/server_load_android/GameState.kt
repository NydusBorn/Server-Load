package com.nydus.example.server_load_android

import android.app.AlertDialog
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.async
import kotlinx.coroutines.awaitAll
import kotlinx.coroutines.delay
import java.lang.Exception
import java.time.Instant
import java.util.PriorityQueue

object GameState {
    var BuildingPriority = mutableFloatStateOf(0.33f)
    var ResearchPriority = mutableFloatStateOf(0.33f)
    var BoostPriority = mutableFloatStateOf(0.33f)
    var FocusedBuilding = mutableIntStateOf(-1)
    var FocusedResearch = mutableIntStateOf(-1)
    var Building_Bits = mutableFloatStateOf(1.0f)
    var Building_Bytes = mutableFloatStateOf(0.0f)
    var Building_Kilo = mutableFloatStateOf(0.0f)
    var Building_Mega = mutableFloatStateOf(0.0f)
    var Building_Giga = mutableFloatStateOf(0.0f)
    var Building_Tera = mutableFloatStateOf(0.0f)
    var Building_Peta = mutableFloatStateOf(0.0f)
    var Building_Exa = mutableFloatStateOf(0.0f)
    var Processes = mutableFloatStateOf(1.0f)
    var Overflows = mutableFloatStateOf(0.0f)
    var Research_Bits_Add = mutableFloatStateOf(0.0f)
    var Research_Bits_Mul = mutableFloatStateOf(0.0f)
    var Research_Bytes_Add = mutableFloatStateOf(0.0f)
    var Research_Bytes_Mul = mutableFloatStateOf(0.0f)
    var Research_Kilo_Add = mutableFloatStateOf(0.0f)
    var Research_Kilo_Mul = mutableFloatStateOf(0.0f)
    var Research_Mega_Add = mutableFloatStateOf(0.0f)
    var Research_Mega_Mul = mutableFloatStateOf(0.0f)
    var Research_Giga_Add = mutableFloatStateOf(0.0f)
    var Research_Giga_Mul = mutableFloatStateOf(0.0f)
    var Research_Tera_Add = mutableFloatStateOf(0.0f)
    var Research_Tera_Mul = mutableFloatStateOf(0.0f)
    var Research_Peta_Add = mutableFloatStateOf(0.0f)
    var Research_Peta_Mul = mutableFloatStateOf(0.0f)
    var Research_Exa_Add = mutableFloatStateOf(0.0f)
    var Research_Exa_Mul = mutableFloatStateOf(0.0f)
    var Research_Process_Mul = mutableFloatStateOf(0.0f)
    var EndGame = mutableFloatStateOf(0.0f)
    var requestQueue = PriorityQueue<ServerRequest>(10){
        o1, o2 -> o1.compareTo(o2)
    }
    var requestResolver = suspend { 
        while (true) {
            if (Connector.Connection != null && requestQueue.isNotEmpty()){
                while (true){
                    val req = requestQueue.poll() ?: break
                    if (req.type == requestQueue.peek()?.type){
                        continue
                    }
                    else{
                        req.function.invoke()
                    }
                }
            }
            else{
                requestQueue.clear()
            }
            delay(100)
            requestQueue.add(ServerRequest(ServerRequestType.update, Instant.now(), ::Update))
        }
    }

    suspend fun Update(){
        awaitAll(GlobalScope.run { 
            async{
                try {
                    Connector.Connection?.getOutputStream()?.write("update".toByteArray())
                    val buffer = ByteArray(16384)
                    val len = Connector.Connection?.getInputStream()?.read(buffer)
                    val res = String(buffer, 0, len!!).split(" ")
                    if (res[0] != "updated") {
                        throw Exception("response incorrect: $res")
                    }
                    BuildingPriority.floatValue = res[1].toFloat()
                    BoostPriority.floatValue = res[2].toFloat()
                    ResearchPriority.floatValue = res[3].toFloat()
                    FocusedResearch.intValue = res[4].toInt()
                    FocusedBuilding.intValue = res[5].toInt()
                    Building_Bits.floatValue = res[6].toFloat()
                    Building_Bytes.floatValue = res[7].toFloat()
                    Building_Kilo.floatValue = res[8].toFloat()
                    Building_Mega.floatValue = res[9].toFloat()
                    Building_Giga.floatValue = res[10].toFloat()
                    Building_Tera.floatValue = res[11].toFloat()
                    Building_Peta.floatValue = res[12].toFloat()
                    Building_Exa.floatValue = res[13].toFloat()
                    Processes.floatValue = res[14].toFloat()
                    Overflows.floatValue = res[15].toFloat()
                    Research_Bits_Add.floatValue = res[16].toFloat()
                    Research_Bits_Mul.floatValue = res[17].toFloat()
                    Research_Bytes_Add.floatValue = res[18].toFloat()
                    Research_Bytes_Mul.floatValue = res[19].toFloat()
                    Research_Kilo_Add.floatValue = res[20].toFloat()
                    Research_Kilo_Mul.floatValue = res[21].toFloat()
                    Research_Mega_Add.floatValue = res[22].toFloat()
                    Research_Mega_Mul.floatValue = res[23].toFloat()
                    Research_Giga_Add.floatValue = res[24].toFloat()
                    Research_Giga_Mul.floatValue = res[25].toFloat()
                    Research_Tera_Add.floatValue = res[26].toFloat()
                    Research_Tera_Mul.floatValue = res[27].toFloat()
                    Research_Peta_Add.floatValue = res[28].toFloat()
                    Research_Peta_Mul.floatValue = res[29].toFloat()
                    Research_Exa_Add.floatValue = res[30].toFloat()
                    Research_Exa_Mul.floatValue = res[31].toFloat()
                    Research_Process_Mul.floatValue = res[32].toFloat()
                    EndGame.floatValue = res[33].toFloat()
                }catch (e: Exception) {
                    Connector.AppMainThread.post {
                        AlertDialog.Builder(Connector.NavController!!.context).setTitle("Error")
                            .setMessage("${e.message.toString()}\n${e.javaClass}\n${e.stackTrace[0]}")
                            .show()
                    }
                }
            }
        })
    }
}