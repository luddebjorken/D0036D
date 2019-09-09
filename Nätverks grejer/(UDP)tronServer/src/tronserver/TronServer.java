/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tronserver;

import java.io.IOException;
import java.net.ServerSocket;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author ludfra-7-local
 */
public class TronServer {

    /**
     * @param args the command line arguments
     */
    String[] names;
    public int players = 0; 
    playerInstance[] pInstance;
    boolean waiting = true;

    public static void main(String[] args){
        TronServer server = new TronServer();
        server.start();
    }
    
    public void start(){
        int port = 25565;
        ServerSocket ss = null;
        names = new String[4];
        pInstance = new playerInstance[4];
        
        try {
            ss = new ServerSocket(port);
        } catch (IOException ex) {
            Logger.getLogger(TronServer.class.getName()).log(Level.SEVERE, null, ex);
        }
        while(true){
            
            if(players<4){
                try {
                    pInstance[players] = new playerInstance(ss, players, this);
                    pInstance[players].start();
                } catch (IOException ex) {
                    Logger.getLogger(TronServer.class.getName()).log(Level.SEVERE, null, ex);
                }
            }else{
                System.out.println("Lobby full!");
            }
            waiting = true;
            while(waiting){
                try {
                    Thread.sleep(50);
                } catch (InterruptedException ex) {
                    Logger.getLogger(TronServer.class.getName()).log(Level.SEVERE, null, ex);
                }
            };
                players++;
        }
        
    }
    
    public void startRound(){
        gameInstance g = new gameInstance(pInstance, players);
        
        for(int i=0; i < players; i++){
            pInstance[i].sendStart();
            pInstance[i].state = 1;
            pInstance[i].game = g;
        }
        g.run();
        
    }
    
}


