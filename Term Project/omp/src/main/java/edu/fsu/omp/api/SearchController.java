package edu.fsu.omp.api;

import edu.fsu.omp.data.ProductDTO;
import edu.fsu.omp.service.ProductSearchService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

import java.util.ArrayList;
import java.util.List;

@Slf4j
@Controller
@RequestMapping(path="/search")
public class SearchController {
    @Autowired
    private ProductSearchService searchService;
    @GetMapping()
    public ResponseEntity<List<ProductDTO>> getProduct(@RequestParam(required = true) String query,
                                                       @RequestParam(required = false) Integer sleep_millis) {
        if(sleep_millis != null) {
            try {
                log.debug("----- search sleeping -----");
                Thread.sleep(sleep_millis);
            } catch (InterruptedException e) {
                log.debug("----- search interrupted, timing out -----");
                return ResponseEntity.status(HttpStatus.REQUEST_TIMEOUT).body(new ArrayList<>());
            }
        }
        return ResponseEntity.status(HttpStatus.OK).body(searchService.search(query));
    }
}
