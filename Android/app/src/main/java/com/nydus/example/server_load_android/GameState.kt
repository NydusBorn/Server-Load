package com.nydus.example.server_load_android

import android.app.AlertDialog
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.Job
import kotlinx.coroutines.async
import kotlinx.coroutines.awaitAll
import kotlinx.coroutines.delay
import java.lang.Exception
import java.time.Instant
import java.util.PriorityQueue
import kotlin.coroutines.createCoroutine

object GameState {
    var BuildingPriority = mutableFloatStateOf(0.33f)
    var ResearchPriority = mutableFloatStateOf(0.33f)
    var BoostPriority = mutableFloatStateOf(0.33f)
    var FocusedBuilding = mutableIntStateOf(-1)
    var FocusedResearch = mutableIntStateOf(-1)
    var DynamicPriority = mutableStateOf(false)
    var PriorityRequestInProgress = false
    var FocusBuildingRequestInProgress = false
    var FocusResearchRequestInProgress = false
    var OverflowRequestInProgress = false
    var Building_Bits = mutableFloatStateOf(1.0f)
    var Building_Bytes = mutableFloatStateOf(0.0f)
    var Building_Kilo = mutableFloatStateOf(0.0f)
    var Building_Mega = mutableFloatStateOf(0.0f)
    var Building_Giga = mutableFloatStateOf(0.0f)
    var Building_Tera = mutableFloatStateOf(0.0f)
    var Building_Peta = mutableFloatStateOf(0.0f)
    var Building_Exa = mutableFloatStateOf(0.0f)
    var Processes = mutableFloatStateOf(1.0f)
    var Overflows = mutableIntStateOf(0)
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
                    if (req.type == requestQueue.peek()?.type || (req.type == ServerRequestType.update && requestQueue.isNotEmpty() && requestQueue.peek()?.type != ServerRequestType.update)){
                        continue
                    }
                    else{
                        req.function.invoke()
                    }
                }
            }
            else{
                if (Connector.Connection == null){
                    break
                }
            }
            delay(100)
            requestQueue.add(ServerRequest(ServerRequestType.update, Instant.now(), ::update))
        }
    }
    var currentHandler: Job? = null

    suspend fun update(){
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
                    if (OverflowRequestInProgress){return@async}
                    if (!PriorityRequestInProgress){
                        BuildingPriority.floatValue = res[1].toFloat()
                        BoostPriority.floatValue = res[2].toFloat()
                        ResearchPriority.floatValue = res[3].toFloat()
                        DynamicPriority.value = res[6] == "1"
                    }
                    if (!FocusResearchRequestInProgress){
                        FocusedResearch.intValue = res[4].toInt()
                    }
                    if (!FocusBuildingRequestInProgress){
                        FocusedBuilding.intValue = res[5].toInt()
                    }
                    Building_Bits.floatValue = res[7].toFloat()
                    Building_Bytes.floatValue = res[8].toFloat()
                    Building_Kilo.floatValue = res[9].toFloat()
                    Building_Mega.floatValue = res[10].toFloat()
                    Building_Giga.floatValue = res[11].toFloat()
                    Building_Tera.floatValue = res[12].toFloat()
                    Building_Peta.floatValue = res[13].toFloat()
                    Building_Exa.floatValue = res[14].toFloat()
                    Processes.floatValue = res[15].toFloat()
                    Overflows.intValue = res[16].toInt()
                    Research_Bits_Add.floatValue = res[17].toFloat()
                    Research_Bits_Mul.floatValue = res[18].toFloat()
                    Research_Bytes_Add.floatValue = res[19].toFloat()
                    Research_Bytes_Mul.floatValue = res[20].toFloat()
                    Research_Kilo_Add.floatValue = res[21].toFloat()
                    Research_Kilo_Mul.floatValue = res[22].toFloat()
                    Research_Mega_Add.floatValue = res[23].toFloat()
                    Research_Mega_Mul.floatValue = res[24].toFloat()
                    Research_Giga_Add.floatValue = res[25].toFloat()
                    Research_Giga_Mul.floatValue = res[26].toFloat()
                    Research_Tera_Add.floatValue = res[27].toFloat()
                    Research_Tera_Mul.floatValue = res[28].toFloat()
                    Research_Peta_Add.floatValue = res[29].toFloat()
                    Research_Peta_Mul.floatValue = res[30].toFloat()
                    Research_Exa_Add.floatValue = res[31].toFloat()
                    Research_Exa_Mul.floatValue = res[32].toFloat()
                    Research_Process_Mul.floatValue = res[33].toFloat()
                    EndGame.floatValue = res[34].toFloat()
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

    suspend fun setPriority(){
        awaitAll(GlobalScope.run {
            async{
                try {
                    PriorityRequestInProgress = false
                    Connector.Connection?.getOutputStream()?.write("set_priority ${BoostPriority.floatValue} ${ResearchPriority.floatValue} ${BuildingPriority.floatValue} ${if (DynamicPriority.value) "1" else "0"}".toByteArray())
                    val buffer = ByteArray(16384)
                    val len = Connector.Connection?.getInputStream()?.read(buffer)
                    val res = String(buffer, 0, len!!)
                    if (res != "ok") {
                        throw Exception("response incorrect: $res")
                    }
                    else {
                        return@async 
                    }
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

    suspend fun setFocusedBuilding(){
        awaitAll(GlobalScope.run {
            async{
                try {
                    FocusBuildingRequestInProgress = false
                    Connector.Connection?.getOutputStream()?.write("set_focus_building ${FocusedBuilding.intValue}".toByteArray())
                    val buffer = ByteArray(16384)
                    val len = Connector.Connection?.getInputStream()?.read(buffer)
                    val res = String(buffer, 0, len!!)
                    if (res != "ok") {
                        throw Exception("response incorrect: $res")
                    }
                    else {
                        return@async
                    }
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

    suspend fun setFocusedResearch(){
        awaitAll(GlobalScope.run {
            async{
                try {
                    FocusBuildingRequestInProgress = false
                    Connector.Connection?.getOutputStream()?.write("set_focus_research ${FocusedResearch.intValue}".toByteArray())
                    val buffer = ByteArray(16384)
                    val len = Connector.Connection?.getInputStream()?.read(buffer)
                    val res = String(buffer, 0, len!!)
                    if (res != "ok") {
                        throw Exception("response incorrect: $res")
                    }
                    else {
                        return@async
                    }
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

    suspend fun overflow(){
        awaitAll(GlobalScope.run {
            async{
                try {
                    OverflowRequestInProgress = false
                    Connector.Connection?.getOutputStream()?.write("overflow".toByteArray())
                    val buffer = ByteArray(16384)
                    val len = Connector.Connection?.getInputStream()?.read(buffer)
                    val res = String(buffer, 0, len!!)
                    if (res != "ok") {
                        throw Exception("response incorrect: $res")
                    }
                    else {
                        return@async
                    }
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