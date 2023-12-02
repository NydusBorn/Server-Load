package com.nydus.example.server_load_android

import java.time.Instant
import kotlin.reflect.KSuspendFunction0

enum class ServerRequestType{
    update, setPriority, setFocusedResearch, setFocusedBuilding, overflow 
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
                    ServerRequestType.setPriority -> {
                        when (other.type){
                            ServerRequestType.setPriority -> 0
                            ServerRequestType.setFocusedResearch -> -1
                            ServerRequestType.setFocusedBuilding -> -2
                            ServerRequestType.overflow -> -3
                            ServerRequestType.update -> -4
                        }
                    }
                    ServerRequestType.setFocusedResearch -> {
                        when (other.type){
                            ServerRequestType.setPriority -> 1
                            ServerRequestType.setFocusedResearch -> 0
                            ServerRequestType.setFocusedBuilding -> -1
                            ServerRequestType.overflow -> -2
                            ServerRequestType.update -> -3
                        }
                    }
                    ServerRequestType.setFocusedBuilding -> {
                        when (other.type){
                            ServerRequestType.setPriority -> 2
                            ServerRequestType.setFocusedResearch -> 1
                            ServerRequestType.setFocusedBuilding -> 0
                            ServerRequestType.overflow -> -1
                            ServerRequestType.update -> -2
                        }
                    }
                    ServerRequestType.overflow -> {
                        when (other.type){
                            ServerRequestType.setPriority -> 3
                            ServerRequestType.setFocusedResearch -> 2
                            ServerRequestType.setFocusedBuilding -> 1
                            ServerRequestType.overflow -> 0
                            ServerRequestType.update -> -1
                        }
                    }
                    ServerRequestType.update -> {
                        when (other.type){
                            ServerRequestType.setPriority -> 4
                            ServerRequestType.setFocusedResearch -> 3
                            ServerRequestType.setFocusedBuilding -> 2
                            ServerRequestType.overflow -> 1
                            ServerRequestType.update -> 0
                        }
                    }
                }
            }
        }
    }
}
