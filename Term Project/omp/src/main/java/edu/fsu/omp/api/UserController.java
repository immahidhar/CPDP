package edu.fsu.omp.api;

import edu.fsu.omp.data.UserDTO;
import edu.fsu.omp.entity.User;
import edu.fsu.omp.repo.UserRepository;
import edu.fsu.omp.util.ModelMapperUtil;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;

@Slf4j
@Controller
@RequestMapping(path="/user")
public class UserController {
    @Autowired
    private UserRepository userRepository;
    @Autowired
    private ModelMapperUtil modelMapper;

    @GetMapping(path="/get_all")
    @ResponseStatus(HttpStatus.OK)
    public @ResponseBody List<UserDTO> getAllUsers() {
        return StreamSupport.stream(userRepository.findAll().spliterator(), false)
                .map(this::convertToDTO).collect(Collectors.toList());
    }

    @GetMapping(path="/get")
    @ResponseStatus(HttpStatus.OK)
    public @ResponseBody List<UserDTO> getUser(@RequestParam(required = false) String id,
                                               @RequestParam(required = false) String username) {
        if(Integer.parseInt(id) != 0)
            return Collections.singletonList(convertToDTO(userRepository.findById(Integer.parseInt(id)).get()));
        else if(username != null)
            return userRepository.findByUsername(username).stream()
                .map(this::convertToDTO).collect(Collectors.toList());
        return null;
    }

    @PostMapping(path="/add")
    @ResponseStatus(HttpStatus.CREATED)
    public @ResponseBody String addNewUser(@RequestBody UserDTO user) {
        userRepository.save(modelMapper.map(user, User.class));
        return "User added";
    }

    @DeleteMapping(path="/delete")
    @ResponseStatus(HttpStatus.OK)
    public @ResponseBody String deleteUser(@RequestParam(required = false) String id,
                                           @RequestParam(required = false) String username) {
        log.debug("Deleting user with id: " + id);
        log.debug("Deleting user with username: " + username);
        if(Integer.parseInt(id) != 0) userRepository.deleteById(Integer.parseInt(id));
        else if(username != null) userRepository.deleteByUsername(username);
        return "User deleted";
    }

    @DeleteMapping(path="/delete_all")
    @ResponseStatus(HttpStatus.ACCEPTED)
    public @ResponseBody String deleteAllUsers() {
        log.info("Deleting all users from database!!!");
        userRepository.deleteAll();
        return "All users deleted";
    }

    private UserDTO convertToDTO(User user) {
        return modelMapper.map(user, UserDTO.class);
    }
}
