package com.nydus.example.server_load_android

import java.time.Instant
import kotlin.reflect.KSuspendFunction0

enum class ServerRequestType{
    update, setBoost, setResearch, setBuilding, setFocusedResearch, setFocusedBuilding 
}
data class ServerRequest(
    val type: ServerRequestType,
    val instant: Instant,
    val function: KSuspendFunction0<Unit>
){
    fun compareTo(other: ServerRequest): Int {
        return if (this.type == other.type){
            this.instant.compareTo(other.instant)
        }
        else {
            return if (this.type == ServerRequestType.update){
                1
            } else if (other.type == ServerRequestType.update) {
                -1
            } else{
                when (this.type){
                    ServerRequestType.setBoost -> {
                        when (other.type){
                            ServerRequestType.setBoost -> 0
                            ServerRequestType.setResearch -> -1
                            ServerRequestType.setBuilding -> -2
                            ServerRequestType.setFocusedResearch -> -3
                            ServerRequestType.setFocusedBuilding -> -4
                            ServerRequestType.update -> -5
                        }
                    }
                    ServerRequestType.setResearch -> {
                        when (other.type){
                            ServerRequestType.setBoost -> 1
                            ServerRequestType.setResearch -> 0
                            ServerRequestType.setBuilding -> -1
                            ServerRequestType.setFocusedResearch -> -2
                            ServerRequestType.setFocusedBuilding -> -3
                            ServerRequestType.update -> -4
                        }
                    }
                    ServerRequestType.setBuilding -> {
                        when (other.type){
                            ServerRequestType.setBoost -> 2
                            ServerRequestType.setResearch -> 1
                            ServerRequestType.setBuilding -> 0
                            ServerRequestType.setFocusedResearch -> -1
                            ServerRequestType.setFocusedBuilding -> -2
                            ServerRequestType.update -> -3
                        }
                    }
                    ServerRequestType.setFocusedResearch -> {
                        when (other.type){
                            ServerRequestType.setBoost -> 3
                            ServerRequestType.setResearch -> 2
                            ServerRequestType.setBuilding -> 1
                            ServerRequestType.setFocusedResearch -> 0
                            ServerRequestType.setFocusedBuilding -> -1
                            ServerRequestType.update -> -2
                        }
                    }
                    ServerRequestType.setFocusedBuilding -> {
                        when (other.type){
                            ServerRequestType.setBoost -> 4
                            ServerRequestType.setResearch -> 3
                            ServerRequestType.setBuilding -> 2
                            ServerRequestType.setFocusedResearch -> 1
                            ServerRequestType.setFocusedBuilding -> 0
                            ServerRequestType.update -> -1
                        }
                    }
                    ServerRequestType.update -> {
                        when (other.type){
                            ServerRequestType.setBoost -> 5
                            ServerRequestType.setResearch -> 4
                            ServerRequestType.setBuilding -> 3
                            ServerRequestType.setFocusedResearch -> 2
                            ServerRequestType.setFocusedBuilding -> 1
                            ServerRequestType.update -> 0
                        }
                    }
                }
            }
        }
    }
}
