/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package tronserver;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.Timer;

/**
 *
 * @author ludfra-7-local
 */
public class gameInstance implements ActionListener {
    Timer timer;
    int tick = 0;
    int delay = 250; //Tickrate in ms
    int[][] pos;
    int[][] vel;
    int[][] field;
    playerInstance[] pInstance;
    int players;
    
    public gameInstance(playerInstance[] p, int players){
        pInstance = p;
        this.players = players;
        
        pos = new int[players][2];
        vel = new int[players][2];
        field = new int[32][32];
        
        timer = new Timer(delay, (ActionListener) this);
        timer.start();
        
        //Sets empty field
        for (int i = 0; i < 32; i++) {
            for (int j = 0; j < 32; j++) {
                field[i][j] = -1;
            }
        }
        
        //Sets default values
        vel[0][0] = 0;
        vel[0][1] = 1;

        pos[0][0] = 0;
        pos[0][1] = 0;

        vel[1][0] = 0;
        vel[1][1] = -1;

        pos[1][0] = 31;
        pos[1][1] = 31;
        
        
        if(players>2){
            vel[2][0] = 1;
            vel[2][1] = 0;
            
            pos[2][0] = 0;
            pos[2][1] = 31;
            
            if(players>3){

                vel[3][0] = -1;
                vel[3][1] = 0;
                
                pos[3][0] = 31;
                pos[3][1] = 0;
            }
        }
        System.out.println("Players: " + players);
        for (int i = 0; i < players; i++) {
            field[pos[i][0]][pos[i][1]] = i;
        }
    }
    
    void run(){
        System.out.println("Game started!!");
        timer.start();
    }
    
    public void actionPerformed(ActionEvent e){
        tick++;
        //Sets field to player color
        int dead = 0;
        String alive = "";
        for (int i = 0; i < players; i++) {
            if(pInstance[i].state==2){
                dead++;
            }else{
                alive+=pInstance[i].name;
            }
        }
        System.out.println("Alive: " + (players-dead) + ", Dead: " + dead);
        if(players-dead == 1){
            for (int i = 0; i < players; i++) {
                pInstance[i].sendWin(alive);
            }
            System.out.println(alive + " wins!");
        }else if(players-dead == 1){
            for (int i = 0; i < players; i++) {
                pInstance[i].sendWin("Nobody ");
            }
        }
            
        for(int i = 0; i < players; i++){
            if(pInstance[i].state!=2){
                field[pos[i][0]][pos[i][1]] = i;
            }
        }
        //Changes players position, sends new positions
        for(int i = 0; i < players; i++){
            pos[i][0] += vel[i][0];
            pos[i][1] += vel[i][1];
            pInstance[i].sendPos(tick);
        }
        
        
    }
    
    
}
