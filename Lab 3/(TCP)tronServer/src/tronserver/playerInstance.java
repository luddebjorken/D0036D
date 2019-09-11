/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tronserver;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author ludfra-7-local
 */
public class playerInstance extends Thread {
    int state = 0; //0=menu, 1=alive, 2=dead, 3=disconneted
    int id;
    String name;
    
    ServerSocket ss;
    Socket ps;
    DataOutputStream out;
    DataInputStream in;
    TronServer t;
    gameInstance game;

    playerInstance(ServerSocket ss, int id, TronServer ts) throws IOException {
        this.id = id;
        this.t = ts;
        this.ss = ss;
        
        System.out.println("Thread #" + id + " started!");
        
        
    }
    
    @Override
    public void run(){
        try{
            ps = ss.accept();
            System.out.println("Client accepted @ " + ps.getInetAddress().getHostAddress() + ":" + ps.getPort());
            out = new DataOutputStream(ps.getOutputStream());
            in = new DataInputStream(ps.getInputStream());

            //Saves name
            while(true){
                String msg = in.readUTF();
                if(msg.startsWith("0")){
                    name = msg.substring(2);
                    t.names[id] = name;
                    break;
                }
            }

            System.out.println("Name: " + name + ":" + id);

            for (int i = 0; i <= t.players; i++) {
                t.pInstance[i].newPlayer();
            }
        }catch(IOException ex){
            Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
        }
        try {
            Thread.sleep(500);
        } catch (InterruptedException ex) {
            Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
        }
        t.waiting = false;
        waitForStart();
    }
    
    public void waitForStart(){
        System.out.println("Thread waiting...");
        while(true){
            try {
                String msg = in.readUTF();
                if(msg.startsWith("1")){
                    if(t.players>0){ //Failsafe
                        t.startRound();
                        gameInput();
                        break;
                    }
                }else if(msg.startsWith("2")){
                    gameInput();
                    break;
                }

            } catch (SocketException ex){ //On socket close
                break; //Terminates player instance
            }
            catch (IOException ex) {
                Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
            } 
            try {
                Thread.sleep(500);
            } catch (InterruptedException ex) {
                Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    public void gameInput(){
        while(state==1){
            try {
                String msg = in.readUTF();
                System.out.println("In: " + msg);
                switch(msg.charAt(0)){
                    case '2':
                        System.out.println("2ASD");
                        if(msg.substring(2,3).equals(id)) {
                            System.out.println("Message from wrong client " + msg.charAt(2)+":"+id);
                            break;
                        } //Cancels if from wrong client (failsafe)
                        msg = msg.substring(msg.indexOf("|")+1);
                        game.vel[id] = new int[]{Integer.parseInt(msg.substring(msg.indexOf('|')+1,msg.indexOf(','))), Integer.parseInt(msg.substring(msg.indexOf(',')+1,msg.length()))};
                        System.out.println(game.vel[id][0] + "===" + game.vel[id][1]);
                        break;
                    case '3':
                        //Send field
                        break;
                }
            } catch (SocketException ex){ //On socket close
                state=3; //Kills player
            } catch (IOException ex) {
                Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    //Method for informing clients of new players. Called whenever new player joins
    public void newPlayer() throws IOException{
        
        //Constructs message string
        String names = "";
        for(int i = 0; i <= t.players; i++){
            names+=t.names[i]+",";
        }
        
        //Sends names of all players
        System.out.println(names);
        out.writeUTF("0|"+ id + "|" +(t.players+1)+"|"+names);
        System.out.println("0|"+ id + "|" +(t.players+1)+"|"+names);
        out.flush();
        
    }
    
    public void sendPos(int tick){
        String velocities = "";
        for (int i = 0; i < t.players; i++) {
            checkDead();
            if(game.pInstance[i].state < 2) velocities+= game.pos[i][0] + "," + game.pos[i][1] + ";" ;
            else velocities+= -10 + "," + -10 + ";" ;
        }
        try {
            out.writeUTF(1 + "|" + tick + "|" + t.players + "|" + velocities);
        } catch (IOException ex) {
            Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    public void sendWin(String name){
        try {
            out.writeUTF(3 + "|" + name);
        } catch (IOException ex) {
            Logger.getLogger(playerInstance.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    void checkDead(){
        if(state<2){
            if(game.pos[id][0]>31 ||game.pos[id][0]<0 ||game.pos[id][1]>31 ||game.pos[id][1]<0 ){
                System.out.println("Player " + id + " has perished!!");
                state = 2;
            }else if(game.field[game.pos[id][0]][game.pos[id][1]]!=-1){
                System.out.println("Player " + id + " has perished!!");
                state = 2;
            }
        }
        
    }
}
